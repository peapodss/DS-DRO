// Most things to do with CPLD communication.
// Also serial... cuz they are kinda intertwined.

int volatile pos1, pos2, pos3;
bool volatile reset1, reset2, reset3;

// DRO (position counters originating from CPLD)
long dro_now[3];
long dro_was[3];
byte dro_ena;
byte dro_zst;

// Serial receive buffer
char srxb[64];
byte volatile srxb_wix, srxb_rix, srxb_len;

// TV horizontal hook runs at F1 = 50*625 (PAL 50 Hz, 625 lines, that's 31250 Hz).
// Transfers nibble(4-bit) counters in CPLD to integer(16-bit) counters.
// Transfers incoming serial data to serial receive buffer;
//
// Care must be taken taken not to exceed the maximum time, or the TV signal will desynchronize.
// Therefore, CPLD communication is split into a state machine and we utilize direct register access for SPI.
// This is low-level code, seen from the Arduino perspective.
//
// CPLD state-machine goes through 4 states, thus runs at F2 = F1/4 (that's 7812.5 Hz)
// The state-machine performs a read-out on all 3 counters in the CPLD, each being 4-bits bits, signed.
// As such, they can handle up to 7 quadrature states (well, -8 to +7 to be exact) between each read-out.
// Maximum quadrature signal frequency should thus be F3 = F2 * 7 (that's 54687.5… Hz)
//
// Encoders are 5µM and will generate a quadrature signal of 33333.333… Hz at 10000mm/m.
// This leaves some headroom for short-term jitter from vibration/other mechanical sources.
//
// Serial communication runs as part of the state-machine and will poll at F2 (that's 7812.5 Hz).
// Each time one char (8-bits) will be read, and with the typical 8N1 format has 10-bits per char.
// This yields an absolute maximum baudrate of F2*10 (that's 78125 bps) for serial communication.

void cpld_serial_hook() {
  static byte state = 0;
  static byte hi = 0, lo = 0;
  static byte phi = 0, plo = 0;
  static char delta1, delta2, delta3;

  // CPLD communication state machine
  if(state == 0) {
    // State 0 - read high byte from CPLD, via SPI, calculate Q#0 delta
    // 7: 0,  6…4: Reset flags Q#0…Q#2,  3…0: Counter Q#0
    phi = hi;
    hi = SPDR;
    delta1 = (((hi - phi) & 0x0F) ^ 0x08) - 0x08;
    SPDR = 0;
    state = 1;
  } else if(state == 1) {
    // State 1 - read low byte from CPLD, via SPI, calculate Q#1…Q#2 deltas
    // 7…4: Counter Q#1,  3…0: Counter Q#2
    fastWrite(Latch, HIGH);
    plo = lo;
    lo = SPDR;
    delta2 = (char)(lo - (plo & 0xF0)) >> 4;
    delta3 = (((lo - plo) & 0x0F) ^ 0x08) - 0x08;
    fastWrite(Latch, LOW);
    SPDR = 0;
    state = 2;
  } else if(state == 2) {
    // State 2 - use deltas and zero flags to update and/or reset 16-bit counters
    if(hi & 0x40) { // Reset was issued for Q#0
      pos1 = hi & 0x0F;
      reset1 = true;
    } else {
      pos1 += delta1;
    }
    if(hi & 0x20) { // Reset was issued for Q#1
      pos2 = lo >> 4;
      reset2 = true;
    } else {
      pos2 += delta2;
    }
    if(hi & 0x10) { // Reset was issued for Q#2
      pos3 = lo & 0x0F;
      reset3 = true;
    } else {
      pos3 += delta3;
    }
    state = 3;
  } else if(state == 3) {
    // State 3 - check for incoming serial data and store in buffer without error/overflow checking
    if(UCSR0A & _BV(RXC0)) {
      byte q = UDR0, nv;
      if((q > 0x1F && q < 0x7F) || q == 0x0A) {
        nv = srxb_wix;
        if(q == 0x0A) {
          srxb_len = nv;
          srxb_wix = sizeof(srxb);
        } else if(nv < sizeof(srxb)) {
          srxb[nv] = q;
          srxb_wix = nv + 1;
        }
      }
    }
    state = 0;
  }
}

// Serial: initialize peripheral
void serial_begin(long baud) {
  UCSR0A = 0;
  UBRR0 = (F_CPU / 8 / baud - 1) / 2;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
}

// Serial: read byte (no error/underflow check)
byte serial_read() {
  byte q = srxb[srxb_rix++];
  srxb_rix &= (sizeof(srxb) - 1);
  return q;
}

// Serial: read byte (no error/underflow check)
char* serial_ptr() {
  return &srxb[srxb_rix];
}

// Serial: write byte (direct, blocking)
void serial_write(byte q) {
  while(!((UCSR0A) & _BV(UDRE0)));
  UDR0 = q;
}

// Serial: return command present
byte serial_available() {
  byte nv = srxb_len & (sizeof(srxb) - 1);
  if(srxb_wix == sizeof(srxb)) {
    // Command received
    if(!nv) {
      // Bad command length, clear buffer
      srxb_len = 0;
      srxb_wix = 0;
      srxb_rix = 0;
    }
  }
  return nv;
}

// Serial: reset command buffer
void serial_reset() {
  serial_write('\n');
  srxb_len = 0;
  srxb_wix = 0;
  srxb_rix = 0;
}

// Start SPI comm towards CPLD
// Initialize control lines
// Hook TVout horizontal clock
void cpld_begin() {
  SPI.begin();
  pinMode(Latch, OUTPUT);
  pinMode(Seek, OUTPUT);
  digitalWrite(Seek, HIGH);
  TV.set_hbi_hook(cpld_serial_hook);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE1));
}

// Transfer integer(16-bit) counters from horizontal hook to long(32-bit) counters (final stage)
void dro_refresh() {
  int temp, delta;
  static int  ppos1, ppos2, ppos3;

  dro_was[0] = dro_now[0];
  dro_was[1] = dro_now[1];
  dro_was[2] = dro_now[2];

  if(dro_ena & 1) {
    temp = pos1;
    cli();
    if(reset1) {
      reset1 = false;
      sei();
      dro_now[0] = 0;
      delta = temp;
      dro_zst |= 1;
    } else {
      sei();
      delta = temp - ppos1;
    }
    dro_now[0] += delta;
    ppos1 = temp;
  } else {
    dro_was[0] = dro_now[0] = 0;
    dro_zst |= 1;
  }

  if(dro_ena & 2) {
    temp = pos2;
    cli();
    if(reset2) {
      reset2 = false;
      sei();
      dro_now[1] = 0;
      delta = temp;
      dro_zst |= 2;
    } else {
      sei();
      delta = temp - ppos2;
    }
    dro_now[1] += delta;
    ppos2 = temp;
  } else {
    dro_was[1] = dro_now[1] = 0;
    dro_zst |= 2;
  }

  if(dro_ena & 4) {
    temp = pos3;
    cli();
    if(reset3) {
      reset3 = false;
      sei();
      dro_now[2] = 0;
      delta = temp;
      dro_zst |= 4;
    } else {
      sei();
      delta = temp - ppos3;
    }
    dro_now[2] += delta;
    ppos3 = temp;
  } else {
    dro_was[2] = dro_now[2] = 0;
    dro_zst |= 4;
  }
}
