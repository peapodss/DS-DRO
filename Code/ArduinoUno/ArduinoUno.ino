// Digital Read-Out system for Arduino UNO.
// Requires external CPLD device.
// See Platform.h for connections to the outworld.
//
// External devices:
//   Small composite car monitor
//   TinyG2 (via Serial/PC)
//   Handheld pendant with 6x buttons & LEDs
//
// Serial protocol:
//
//  Generally:
//    * Format is 9600 bps, 8-N-1.
//    * All values are upper-case hexadecimal.
//    * Hexadecimal values are sent big-endian.
//    * Commands and responses are ended with '\n'.
//    * Commands are acknowledged with '\n' (empty line).
//    * Commands may not be sent until acknowledge is received.
//    * Commands and responses have a maximum length of 63 characters.
//    * The status response will be automatically sent whenever status changes.
//
//  Commands (PC->Arduino):
//
//    Ts              Set on-screen text field. s = string where '$' is newline, max 3 lines. ex: "THello$World\n"
//    Lxx             Set pendant LEDs. xx = bitfield [5…0] led-on left-right. ex: "L12\n" == X----X
//    Baabbccddeeff   Set button menu. aa…ff are 6x two-char menu text left-right. "  " = no item. "X " = centered X. ex: "BOPJO    ZCMC\n"
//    Vxx             Enable velocity-drop notification (axis physically stopped). xx = velocity / 100 ex: "V01" == 1600 mm/s.
//    v               Disable velocity-drop notification
//    Z               Zero out user coordinate system at current machine coordinate. ex: "Z\n"
//    H               Enable homing mode. ex: "H\n"
//    h               Disable homing mode. ex: "h\n"
//    M               Set machine-coordinate display mode. ex: "M\n"
//    m               Set user-coordinate display mode. ex "m\n"
//    R               Request current read-out. ex "R\n"
//    S               Request current status. ex "S\n"
//
// Responses (Arduino->PC):
//
//    (Xnnnn)(Ynnnn)(Znnnn) Read-out response. Axis not sent if disabled. Signed 16-bit * 100. "X13EFYCA10\n" == X-4.63mm Y+4.28mm
//    Snn                   Status response. nn = bitfield [4] all axes homed [3] machine coords [2…0] X-, Y-, Z-axis 1: homed 0: searching
//    Va                    Velocity drop notification - axis velocity has dropped below threshold. a = axis 'X', 'Y' or 'Z'.

// Libraries
#include <SPI.h>        // Prerequisite for "SerialCPLD"
#include <Wire.h>       // Prerequisite for "Pendant"

// Proper includes
#include "Composite.h"> // Interface with external composite monitor

// Which axes to enable (only recommend disabling Z)
#define DRO_EN (DRO_EN_X | DRO_EN_Y)

#define DRO_EN_X 1
#define DRO_EN_Y 2
#define DRO_EN_Z 4

// Modified TV-out library
TVout TV;

// Impure includes (contains code, not just definitions)
#include "Font5x8.h"    // Custom font
#include "Platform.h"   // Defines platform constants and helper functions
#include "Pendant.h"    // Interface with handheld pendant
#include "SerialCPLD.h" // Interface with Serial and CPLD (quadrature counting)

// DRO system state
bool homing_enabled = true;
bool machine_coords = false;
bool coords_valid = false;
byte velocity_drop = 0;
byte velocity_trig = 0;

// Texts
PROGMEM const char * const xyztext = "a: ###.## mm ##### mm/s";
PROGMEM const char * const ftext = "[  ][  ][  ] ##### mm/s";

// DRO offset and speed holding registers
long dro_ofs[3];
int  dro_spd[3];

// Draws a menu button
//  ix: button index (0=left-most…5=right-most)
//  text: pointer to string of two characters
//    if both are ' ', button is cleared
//    if second is ' ', first char is centered
void drawButton(byte ix, char *text) {
  char two[3];
  if(ix > 2) ix++;
  for(byte y = 85; y < 96; y++) TV.draw_row(y, ix * 17, ix * 17 + 16, 0);
  byte x = ix * 17;
  two[0] = text[0]; two[1] = text[1]; two[2] = 0;
  bool color = two[0] != ' ' || two[1] != ' ';
  TV.draw_line(x +  1, 86, x     , 95, color);
  TV.draw_line(x +  2, 85, x + 15, 85, color);
  TV.draw_line(x + 15, 86, x + 16, 95, color);
  if(two[1] == ' ') {
    two[1] = 0;
    TV.print(x + 6, 87, two);
  } else {
    TV.print(x + 4, 87, two);
  }
}

// Centers up to three lines of text in the textual (middle) area of the screen
//   text: pointer to text, no termination, '$' as new-line
//   len: number of chars to read from *text
void centerText(char *text, byte len) {
  byte lines = len == 0 ? 0 : 1;
  char* line[3] = {text};
  char linelen[3] = {0,0,0};
  byte n;
  
  // Count lines and chars for centering
  for(n = 0; n < len; n++) {
    if(text[n] == '$') {
      text[n] = 0;
      line[lines] = &text[n + 1];
      lines++;
      if(lines == 4) return;
    } else {
      linelen[lines - 1]++;
    }
  }
  text[n] = 0;
  byte ystart = 65 - (lines * 6);
  
  // Clear previous texts
  for(byte y = 47; y < 83; y++) TV.draw_row(y, 0, 119, 0);

  // Draw new texts
  for(n = 0; n < lines; n++) {
    TV.print(60 - ((linelen[n] * 5) / 2), ystart + (n * 12), line[n]);
  }
}

// Main loop
void loop() {
  byte i, j;
  static byte butts_was = 0x00;
  static byte state_was = 0xFF;

  // Read IO (handheld pendant) button register
  byte butts = io_read();
  byte butts_event = (butts ^ butts_was);
  butts_was = butts;
  if(butts_event) for(i = 0; i < 7; i++) {
    if(butts_event & 0x40) {
      // Send button event over serial, examples:
      // "+1\n" = button 1 pressed
      // "-3\n" = button 3 released
      serial_write((butts & 0x40) ? '+' : '-');
      serial_write('0' + i);
      serial_write('\n');
    }
    butts_event <<= 1;
    butts <<= 1;
  }

  // Check if command is available
  // Will return 0 unless a full command line is present
  byte len = serial_available();
  if(len) {
    switch(serial_read()) {
      case 'T': // set text message
        centerText(serial_ptr(), len - 1);
        break;
      case 'L': // set button leds
        if(len != 3) break;
        i = serial_read();
        if(i < '0' && i > '9' && i < 'A' && i > 'F') break;
        i = i > '9' ? (i - 'A' + 10) : i - '0';
        j = serial_read();
        if(j < '0' && j > '3') break;
        i |= (j - '0') << 4;
        io_write(i);
        break;
      case 'B': // set button menu layout
        if(len != 13) break;
        for(i = 0; i < 6; i++) {
          drawButton(i, serial_ptr() + i * 2);
        }
        break;
      case 'Z': // zero user coordinates
        dro_ofs[0] = dro_now[0];
        dro_ofs[1] = dro_now[1];
        dro_ofs[2] = dro_now[2];
        machine_coords = false;
        break;
      case 'H': // enable homing
        if(len != 1) break;
        dro_zst = 0;
        homing_enabled = true;
        digitalWrite(Seek, true);
        break;
      case 'h': // disable homing
        if(len != 1) break;
        dro_zst = 7;
        homing_enabled = false;
        digitalWrite(Seek, false);
        break;
      case 'M': // set machine coordinates
        if(len != 1) break;
        machine_coords = true;
        break;
      case 'm': // set user coordinates
        if(len != 1) break;
        machine_coords = false;
        break;
      case 'R': // request read-out
        if(len != 1) break;
        // Send dro over serial, examples:
        // "X13EFYCA10\n" = X at -4.63mm(0xFE31), Y at +4.28mm(0x01AC), Z disabled
        // "Z0721\n" = X disabled, Y disabled, Z at +47.20mm(0x1270)
        for(i = 0; i < 3; i++) {
          if(dro_ena & (1 << i)) {
            int val = dro_now[i] >> 1;
            serial_write('X' + i);
            serial_write(pgm_read_byte(&hex[val & 0xF])); val >>= 4;
            serial_write(pgm_read_byte(&hex[val & 0xF])); val >>= 4;
            serial_write(pgm_read_byte(&hex[val & 0xF])); val >>= 4;
            serial_write(pgm_read_byte(&hex[val & 0xF]));
          }
        }
        serial_write('\n');
        break;
      case 'S': // request status
        if(len != 1) break;
        state_was = 0xFF; // force status update
        break;
      case 'V': // set velocity drop limit
        if(len != 3) break;
        i = serial_read();
        if(i < '0' && i > '9' && i < 'A' && i > 'F') break;
        i = i > '9' ? (i - 'A' + 10) : i - '0';
        j = serial_read();
        if(j < '0' && j > '9' && j < 'A' && j > 'F') break;
        j = j > '9' ? (j - 'A' + 10) : j - '0';
        velocity_drop = i | (j << 4);
        velocity_trig = 0;
        break;
      case 'v': // disable velocity drop
        velocity_drop = 0;
        velocity_trig = 0;
        break;
      default:
        break;
    }
    serial_reset();
  }

  // Refresh DRO (digital read-out) counters from CPLD
  dro_refresh();

  // Calculate time delta, needed for feedrate calculation
  static int ptm;
  int tm = TV.millis();
  int td = tm - ptm;
  ptm = tm;

  // Check for and handle homing complete condition
  if(homing_enabled && !(dro_zst ^ 0x07)) {
    coords_valid = true;       // machine coordinates are valid
    digitalWrite(Seek, false); // signal cpld to stop homing
    homing_enabled = false;    // homing function completed
    machine_coords = true;     // set machine coordinate display
  }

  // Delay to minimize flicker
  TV.wait_for_vsync();

  // Construct state bitfield, contents:
  // [7…5] reserved
  //    [4] 1: machine coordinates valid (all axes homed)
  //    [3] display is in 1:machine/0:user coordinate mode
  // [2…0] X-, Y-, Z-axis zero mark 0:searching/1:found
  byte state = ((dro_zst & 7) | (coords_valid ? 16 : 0) | (machine_coords ? 8 : 0));
  // Send state updates over serial
  if(state != state_was) {
    // Send state over serial, examples:
    // "S13\n" = 
    serial_write('S');
    serial_write(pgm_read_byte(&hex[state & 15]));
    serial_write(pgm_read_byte(&hex[state >> 4]));
    serial_write('\n');
    state_was = state;
    // Refresh on-screen state flags
    if(! coords_valid) TV.print(5 * 5, 36, "CI"); else TV.print(1 * 5, 36, "  ");
    if(dro_zst ^ 0x07) TV.print(1 * 5, 36, "HE"); else TV.print(5 * 5, 36, "  ");
    if(machine_coords) TV.print(9 * 5, 36, "MC"); else TV.print(9 * 5, 36, "  ");
  }

  // Refresh on-screen DRO
  // Handle velocity drop triggers
  char text[8];
  for(i = 0; i < 3; i++) {
    if(dro_ena & (1 << i)) {
      // Print position
      long val = dro_now[i];
      if(!machine_coords) val -= dro_ofs[i];
      posout(text, val >> 1);
      TV.print( 2 * 5, i * 12, text);
      // Calculate velocity
      val = (abs(dro_now[i] - dro_was[i]) * 300) / td;
      // Do velocity drop notifactions
      if(velocity_drop) {
        if(val / 100 > velocity_drop) velocity_trig |= (1 << i);
        else if(velocity_trig & (1 << i)) {
          // Send velocity drop notification
          serial_write('V');
          serial_write(i == 0 ? 'X' : (i == 1 ? 'Y' : 'Z'));
          serial_write('\n');
          velocity_trig ^= (1 << i);
        }
      }
      // Print velocity
      dro_spd[i] += (val - dro_spd[i]) >> 1; // >> 2; // smooth
      unsout(text, dro_spd[i], 5);
      TV.print(13 * 5, i * 12, text);
      TV.print(114, i * 12, ((dro_zst & (1 << i)) || (tm & 256)) ? ' ' : '*');
    }
  }
  // Print feedrate (X+Y velocities together)
  long val = isqrt(((uint32_t)dro_spd[0]*(uint32_t)dro_spd[0])+((uint32_t)dro_spd[1]*(uint32_t)dro_spd[1]));
  unsout(text, val, 5);
  TV.print(13 * 5, i * 12, text);
}

void setup() {
  // Enable X and Y (not Z)
  dro_ena = DRO_EN;

  TV.begin();              // init Composite output
  TV.select_font(font5x8); // font <= 4x6 pixel fixed-width
  serial_begin(9600);      // requires cpld_begin
  cpld_begin();            // init CPLD communication
  io_begin();              // init communication with control pendant

  // Initialize display
  if(dro_ena & DRO_EN_X) {
    TV.print(0, 0, xyztext); // X axis readout
    TV.print(0, 0, "X");
  }
  if(dro_ena & DRO_EN_Y) {
    TV.print(0, 12, xyztext); // Y axis readout
    TV.print(0, 12, "Y");
  }
  if(dro_ena & DRO_EN_Z) {
    TV.print(0, 24, xyztext); // Z axis readout
    TV.print(0, 24, "Z");
  }
  TV.print(0, 36, ftext); // feedrate readout
  TV.draw_row(44, 0, 119, 1); // dividers
  TV.draw_row(84, 0, 119, 1);
}
