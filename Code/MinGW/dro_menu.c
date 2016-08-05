#include "dro_menu.h"
#include "bridge.h"
#include "menus.h"
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

#define HOME_FEED "F2000"


#define AXIS_X 0x01
#define AXIS_Y 0x02
#define AXIS_Z 0x04

// Workspace size (not including Z switches)
#define WORK_SIZE_X 374
#define WORK_SIZE_Y 570
#define WORK_SIZE_Z 200 // Not connected

// Frame size (must be exact or slightly above hard limits)
#define FRAME_SIZE_X 400
#define FRAME_SIZE_Y 600
#define FRAME_SIZE_Z 200 // Not connected

// Movement direction for the homing operation on each axis
// Can be "-" or "+"/""
#define HOME_DIRECTION_X "-"
#define HOME_DIRECTION_Y "-"
#define HOME_DIRECTION_Z "-"

// Backoff and origin of machine coordinate system
// Sign must be opposite of the HOME_DIRECTION_axis sign
#define HOME_BACKOFF_X 1
#define HOME_BACKOFF_Y 1
#define HOME_BACKOFF_Z 1

#define SH(STR) #STR
#define S(STR) SH(STR)

// Machine state shadows
static bool homing_enabled, machine_coords, coords_valid;
static unsigned char zeroed_axes, zeroed_axes_was, homed_axes; // bitfield
static double dro_x, dro_y, dro_z;

// Menu states
static bool homing, jogging, copying;
static int  jog_axis;
static int  jog_mode;
static uint8_t monitor_axis;

// Menus
static void rootMenu();
static void jogMenu();
static void opMenu();

void stopMotion() {
	send_to_tinyg("!");
	send_to_tinyg("%\n");
	send_to_tinyg("G0\n");
	monitor_velocity(0); // disable monitoring
	monitor_axis = 0; // discard late velocity notifications
}

/* == MENU: CONFIRM & NOTIFY ================================================================== */

void (*yes_handler)();
void (*no_handler)();
void (*ok_handler)();
void *menu_was;
bool is_note;
char confirm_text[63];

static void notifyOk() {
	menuSet(menu_was);
	if(ok_handler) ok_handler();
}

static void confirmYes() {
	menuSet(menu_was);
	if(yes_handler) yes_handler();
}

static void confirmNo() {
	menuSet(menu_was);
	if(no_handler) no_handler();
}

static void confirmMenu() {
	menuSet(confirmMenu);
	set_text(confirm_text);
	set_leds(0);
	menuClear();
	if(is_note) {
		menuSetItem(5, "OK", notifyOk);
	} else {
		menuSetItem(4, "NO", confirmNo);
		menuSetItem(5, "YE", confirmYes);
	}
}

static void confirm(char *question, void (*yes)(), void (*no)()) {
	is_note = false;
	yes_handler = yes;
	no_handler = no;
	menu_was = menuGet();
	strcpy(confirm_text, question);
	confirmMenu();
}

static void notify(char *note, void (*ok)()) {
	is_note = true;
	ok_handler = ok;
	menu_was = menuGet();
	strcpy(confirm_text, note);
	confirmMenu();
}

/* == MENU: ROOT ============================================================================== */

// Response Yes - zero out user coordinates
static void zeroUserCoordinatesYes() {
	zero_coords();
}

// Hardkey event - zero out user coordinates
static void zeroUserCoordinates() {
	confirm("ZERO USER COORDINATES$Confirm?", zeroUserCoordinatesYes, NULL);
}

// Hardkey event - toggle machine/user coordinate display
static void machineCoordinates() {
	if(!machine_coords) {
		enable_machine_coords();
	} else {
		disable_machine_coords();
	}
}

// Menu handler
static void rootMenu() {
	set_text("DIGITAL READ-OUT AND$CNC OVERLORD");
	menuSet(rootMenu);
	set_leds(machine_coords ? 0x01 : 0);
	menuClear();
	menuSetItem(0, "OP", opMenu);
	if(!homing_enabled) {
		// user menus
		menuSetItem(1, "JO", jogMenu);
	}
	menuSetItem(4, "ZC", zeroUserCoordinates);
	if(coords_valid) {
		menuSetItem(5, "MC", machineCoordinates);
	}
}


/* == MENU: OPERATIONS ======================================================================== */

// Final copy coordinates stage
static void copyCoordsLastCycle() {
	char cmd[64];
	sprintf(cmd, "G28.3");
	if(homed_axes & AXIS_X) sprintf(cmd, "%s X%f", cmd, dro_x);
	if(homed_axes & AXIS_Y) sprintf(cmd, "%s Y%f", cmd, dro_y);
	if(homed_axes & AXIS_Z) sprintf(cmd, "%s Z%f", cmd, dro_z);
	sprintf(cmd, "%s\n", cmd);
	send_to_tinyg(cmd);
	copying = false;
	menuSet(rootMenu);
}

// Response Yes - copy coordinates
static void copyCoordsYes() {
	copying = true;
	menuRefresh();
	request_dro();
}

// Hardkey event - copy coordinates
static void copyCoords() {
	confirm("COPY COORDINATES$DRO MCS => TinyG MCS?", copyCoordsYes, NULL);
}

// Response Yes - perform homing cycle
static void goHomeYes() {
	// Will trigger status update for all non-disabled axes
	// This update (when homing==true) will trigger homingNextCycle via on_state_change();
	enable_homing();
	homing = true;
	
	// Disable soft limits (or we might hit them while homing)
	send_to_tinyg("$sl=0");
	
	// Set relative movements
	send_to_tinyg("F91");
	
	// Monitor velocity for blockage
	monitor_velocity(1); // 100 mm/m
	
	homed_axes = 0;
	menuRefresh();
}

// Notification - axis has physically stopped
static void homingFault() {
	stopMotion();
	homing = false;
	monitor_velocity(0); // disable
	disable_homing();
	notify("ERROR - Axis BLOCKED!$Homing was aborted.", NULL);
}

// Hardkey event - perform homing cycle
static void goHome() {
	confirm("GO HOME$Begin homing cycle?", goHomeYes, NULL);
}

// Intermediate homing stages
static void homingNextCycle() {
	printf("SS %u\n", zeroed_axes);
	stopMotion();
	printf("next: stop\n");
	monitor_velocity(1); // 100 mm/m
	Sleep(500);
	if(zeroed_axes == 0x07) {
		printf("next: all\n");
		// All axes homed
		monitor_velocity(0); // disable
		// Will trigger DRO
		// This DRO (when homing==true) will trigger homingLastCycle via on_readout();
		request_dro();
	} else if((zeroed_axes & AXIS_Z) == 0) {
		printf("next: z\n");
		// These will trigger axis-zeroing
		// This zeroing (when homing==true) will trigger homingNextCycle via on_state_change();
		send_to_tinyg("G1 Z" HOME_DIRECTION_Z S(FRAME_SIZE_Z) " " HOME_FEED "\n");
		homed_axes |= AXIS_Z;
		monitor_axis = 3;
	} else if((zeroed_axes & AXIS_Y) == 0) {
		printf("next: y\n");
		send_to_tinyg("G1 Y" HOME_DIRECTION_Y S(FRAME_SIZE_Y) " " HOME_FEED "\n");
		homed_axes |= AXIS_Y;
		monitor_axis = 2;
	} else if((zeroed_axes & AXIS_X) == 0) {
		printf("next: x\n");
		send_to_tinyg("G1 X" HOME_DIRECTION_X S(FRAME_SIZE_X) " " HOME_FEED "\n");
		homed_axes |= AXIS_X;
		monitor_axis = 1;
	}
}

// Final homing stage
static void homingLastCycle() {
	char cmd[64];

	// Set absolute mode
	send_to_tinyg("G90\n");
	
	// Set machine coordinate system (+1 all axes to clear Z mark/switch)
	sprintf(cmd, "G28.3");
	if(homed_axes & AXIS_X) sprintf(cmd, "%s X%f", cmd, dro_x);
	if(homed_axes & AXIS_Y) sprintf(cmd, "%s Y%f", cmd, dro_y);
	if(homed_axes & AXIS_Z) sprintf(cmd, "%s Z%f", cmd, dro_z);
	sprintf(cmd, "%s\n", cmd);
	send_to_tinyg(cmd);

	// Set $home = "homed"
	// Enables the use of software limits
	send_to_tinyg("$home=1");
	
	sprintf(cmd, "G53 G0");
	if(homed_axes & AXIS_X) sprintf(cmd, "%s X" S(HOME_BACKOFF_X), cmd);
	if(homed_axes & AXIS_Y) sprintf(cmd, "%s Y" S(HOME_BACKOFF_Y), cmd);
	if(homed_axes & AXIS_Z) sprintf(cmd, "%s Z" S(HOME_BACKOFF_Z), cmd);
	sprintf(cmd, "%s\n", cmd);
	send_to_tinyg(cmd);

	// Set up software limits
	// Enables the use of software limits
	if(homed_axes & AXIS_X) {
		send_to_tinyg("$xtn=" S(HOME_BACKOFF_X));
		send_to_tinyg("$xtm=" S(WORK_SIZE_X));
	} else {
		send_to_tinyg("$xtn=-" S(FRAME_SIZE_X));
		send_to_tinyg("$xtm=+" S(FRAME_SIZE_X));
	}
	if(homed_axes & AXIS_Y) {
		send_to_tinyg("$ytn=" S(HOME_BACKOFF_Y));
		send_to_tinyg("$ytm=" S(WORK_SIZE_Y));
	} else {
		send_to_tinyg("$ytn=-" S(FRAME_SIZE_Y));
		send_to_tinyg("$ytm=+" S(FRAME_SIZE_Y));
	}
	if(homed_axes & AXIS_Z) {
		send_to_tinyg("$ztn=" S(HOME_BACKOFF_Z));
		send_to_tinyg("$ztm=" S(WORK_SIZE_Z));
	} else {
		send_to_tinyg("$ztn=-" S(FRAME_SIZE_Z));
		send_to_tinyg("$ztm=+" S(FRAME_SIZE_Z));
	}

	// Enable software limits
	send_to_tinyg("$sl=1");
	
	monitor_velocity(0);
	homing = false;

	menuSet(rootMenu);
}

// Hardkey event - toggle homing enabled
static void homingEnabled() {
	if(!homing_enabled) {
		enable_homing();
	} else {
		disable_homing();
		if(homing) {
			stopMotion();
			// Abort homing cycle
			monitor_velocity(0); // disable
			homing = false;
			// Set absolute mode
			send_to_tinyg("G90\n");
			// Check with user
			notify("Warning: Home aborted$Limits left disabled", NULL);
		}
	}
}

static void opMenu() {
	menuSet(opMenu);

	if(!homing) set_text("HOMING OPERATIONS");
	else set_text("HOMING... PLEASE HOLD");
	
	set_leds(homing_enabled ? 0x01 : 0);
	menuClear();
	if(!(homing | homing_enabled)) {
		menuSetItem(0, "<<", rootMenu);
		menuSetItem(1, "GH", goHome);
		menuSetItem(2, "CC", copyCoords);
	}

	menuSetItem(5, "HE", homingEnabled);
	
}


/* == MENU: JOGGING =========================================================================== */

// Hardkey event - switch between axes
static void nextJogAxis() {
	jog_axis = jog_axis == 2 ? 0 : jog_axis + 1;
	jogMenu();
}

// Hardkey event - switch between modes
static void nextJogMode() {
	jog_mode = jog_mode == 5 ? 0 : jog_mode + 1;
	jogMenu();
}

// Notification - axis has physically stopped
static void jogFault() {
	stopMotion();
	jogging = false;
	notify("ERROR - Axis BLOCKED!$Jogging was aborted.", NULL);
}

static void jogDir(char dir) {
	char cmd[20], axis;
	unsigned int max_length;
	send_to_tinyg("$sl=0");
	send_to_tinyg("G91");
	switch(jog_axis) {
		case  0: axis = 'X'; max_length = FRAME_SIZE_X; break;
		case  1: axis = 'Y'; max_length = FRAME_SIZE_Y; break;
		default: axis = 'Z'; max_length = FRAME_SIZE_Z; break;
	}
	switch(jog_mode) {
		case  0: sprintf(cmd, "G0 %c%c0.01\n", axis, dir); break;
		case  1: sprintf(cmd, "G0 %c%c0.1\n", axis, dir);  break;
		case  2: sprintf(cmd, "G0 %c%c1\n", axis, dir);    break;
		case  3: sprintf(cmd, "G0 %c%c10\n", axis, dir);   break;
		case  4: sprintf(cmd, "G1 %c%c%u F3200\n", axis, dir, max_length); break;
		default: sprintf(cmd, "G1 %c%c%u F400\n", axis, dir, max_length);  break;
	}
	send_to_tinyg(cmd);
	jogging = jog_mode > 3; // only continous modes
	if(jogging) {
		// Monitor velocity for blockage
		monitor_velocity(1); // 100 mm/m
		monitor_axis = jog_axis + 1;
	}
}

// Hardkey event - jog in positive direction
static void jogPos() {
	jogDir('+');
}

// Hardkey event - jog in negative direction
static void jogNeg() {
	jogDir('-');
}

// Hardkey event - stop any current jogging
static void jogStop() {
	if(!jogging) return;
	stopMotion();
	send_to_tinyg("$sl=1\n");
	send_to_tinyg("G90");
	jogging = false;
}

// Hardkey event - exit jog menu
static void exitJog() {
	jogStop();
	rootMenu();
}

// Jog menu
static void jogMenu() {
	char txt[63];
	char *axis_str, axis_chr;
	char *mode_str, *mode_str2;
	switch(jog_axis) {
		case  0: axis_str = "X "; axis_chr = 'X'; break;
		case  1: axis_str = "Y "; axis_chr = 'Y'; break;
		default: axis_str = "Z "; axis_chr = 'Z'; break;
	}
	switch(jog_mode) {
		case  0: mode_str = "0.01 mm"; mode_str2 = "00"; break;
		case  1: mode_str = "0.1 mm "; mode_str2 = "01"; break;
		case  2: mode_str = "1 mm   "; mode_str2 = "1 "; break;
		case  3: mode_str = "10 mm  "; mode_str2 = "10"; break;
		case  4: mode_str = "FAST   "; mode_str2 = "FA"; break;
		default: mode_str = "SLOW   "; mode_str2 = "SL"; break;
	}
	sprintf(txt, "Jogging: %c      $Mode   : %s", axis_chr, mode_str);
	set_text(txt);
	menuSet(jogMenu);
	set_leds(0);
	menuClear();
	menuSetItem(0, "<<", exitJog);
	menuSetItem(1, axis_str, nextJogAxis);
	menuSetItem(2, mode_str2, nextJogMode);
	menuSetItem(3, "--", jogNeg);
	menuSetItem(4, "! ", jogStop);
	menuSetItem(5, "++", jogPos);
}


/* == EXTERNAL EVENTS FROM BRIDGE ============================================================= */

// called at menu start-up
void menu_setup() {
	rootMenu();
}

// called periodically
void menu_loop() {
	static bool joggle = 1;
	static bool jog_was = 0;
	if(jogging) {
		joggle = jog_was == 0 ? true : !joggle;
		set_leds(joggle ? 16 : 0);
	} else if(jog_was) {
		menuRefresh();
	}
	jog_was = jogging;
}


/* == EXTERNAL EVENTS FROM DEVICE ============================================================= */

// called when device reports a state change
void on_state_change(uint8_t state) {
	zeroed_axes = state & 7;
	if(zeroed_axes != zeroed_axes_was) {
		if(homing) {
			homingNextCycle();
		}
		zeroed_axes_was = zeroed_axes;
	}
	coords_valid = !!(state & 16);
	machine_coords = !!(state & 8);
	homing_enabled = !!(zeroed_axes ^ 7);
	menuRefresh();
}

// called when user releases a button
void on_button_release(uint8_t button) {
	button = ~button;
	jogStop();
}

void on_readout(double x, double y, double z) {
	dro_x = x;
	dro_y = y;
	dro_z = z;
	if(homing) homingLastCycle();
	if(copying) copyCoordsLastCycle();
}

void on_velocity_drop(uint8_t axis) {
	if(axis + 1 == monitor_axis) {
		if(homing) homingFault();
		if(jogging) jogFault();
	}
}