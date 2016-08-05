
/*

 Arduino interface for TinyG

*/

#include "bridge.h"
#include "websocket.h"
#include "serial.h"
#include "dro_menu.h"
#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define CYCLE_TIME_MICROSECONDS 100000    // time of each main loop iteration

static bool tinyg_connected;
static bool device_connected;
static bool quit;
static char *dev_name;
static char *spjs_name;
static char *spjs_host;
static char *spjs_port;
static char default_spjs_host[] = "localhost";
static char default_spjs_port[] = "8989";
static uint8_t device_leds;
static char device_menu[12];
static char device_text[63];
static bool can_send;
static unsigned char send_home;
static unsigned char send_zero;
static unsigned char send_coord;
static unsigned char send_dro;
static unsigned char velocity_drop;
static Queue cmd_queue;

void set_leds(uint8_t leds) { device_leds = leds; }
void set_menu(char *menu) { memcpy(device_menu, menu, 12); }
void set_text(char *text) { if(strlen(text) < 63) strcpy(device_text, text); }
void clear_menu() { memset(device_menu, ' ', 12); }
void set_menu_item(uint8_t ix, char *item) { memcpy(&device_menu[ix * 2], item, 2); }
void clear_menu_item(uint8_t ix) { memcpy(&device_menu[ix * 2], "  ", 2); }
void enable_homing() { send_home = 2; }
void disable_homing() { send_home = 1; }
void enable_machine_coords() { send_coord = 2; }
void disable_machine_coords() {	send_coord = 1; }
void zero_coords() { send_zero = 1; }
void request_dro() { send_dro = 1; }
void monitor_velocity(uint8_t limit) { velocity_drop = limit; }

// Sends a single command to tinyg as fast as possible
void send_to_tinyg(char *command) {
	char cmd[MAX_CMD_LENGTH];
	int num_cmds_in_queue, num_cmds_sent;
	
	sprintf(cmd, "send %s %s", spjs_name, command);
	cmd_queue.push(&cmd_queue, cmd);

	// send all queued commands
	if(tinyg_connected) {
		num_cmds_in_queue = cmd_queue.size;
		num_cmds_sent = websocket_send_cmds( &cmd_queue );
		if(num_cmds_sent != num_cmds_in_queue) {
			tinyg_connected = false;
		}
	}	
}

// A helper procedure to reset the program and cause the connection
// to the websocket and to the shuttle device to be re-initialized.
static void reset_connections() {
    fprintf(stderr, "============ Reinitializing connections\n");
    cmd_queue.clear( &cmd_queue );
    device_connected = false;
    tinyg_connected = false;
}

// Print a locally enumerated port to screen
static void print_port(char *name,char *device) {
	printf(" * %s %s\n", device, name);
}

// Read ASCII HEXadecimal Big-Endian string
static int16_t read_hex_be(char *ptr, uint8_t len) {
	uint16_t dec = 0;
	uint8_t sh = 0;
	char q;
	while(len--) {
		q = *ptr++;
		if(q >= '0' && q <= '9') q -= '0';
		else if(q >= 'A' && q <= 'F') q -= ('A' - 10);
		else if(q >= 'a' && q <= 'f') q -= ('a' - 10);
		else q = 0;
		dec |= ((uint16_t)q) << sh;
		sh += 4;
	}
	//printf("rhex:%i\n",(int16_t)dec);
	return (int16_t)dec;
}

// -- monsters be here --
int main(int argc, char **argv) {
    char host[256];
    char port[16];
	char cmd[64];
	HANDLE timer; 
	LARGE_INTEGER ft;
	uint8_t previous_leds = 0x00;
	uint8_t previous_drop = 0x00;
	char previous_menu[12] = "            ";
	char previous_text[63] = "";
	char* test;
	char ser[64];
	double dro[3] = {0, 0, 0};
	uint8_t ser_ix = 0;
	int n;
  
	// handle arguments
	if (argc  < 3) {
		fprintf(stderr, "usage: %s <device> <spjs tinyg> (<spjs server> (<spjs port>))\n\n", argv[0] );
		fprintf(stderr, "server defaults to %s\nport defaults to %s\n\n", default_spjs_host, default_spjs_port);
		fprintf(stderr, "example: %s COM13 COM4 \n", argv[0] );
		return 1;
    }
    dev_name = argv[1];
	spjs_name = argv[2];
	spjs_host = argc > 3 ? argv[3] : default_spjs_host;
	spjs_port = argc > 4 ? argv[4] : default_spjs_port;
	fprintf(stderr, "Launching with arguments: %s %s %s %s\n\n", dev_name, spjs_name, spjs_host, spjs_port);

	// initialize WebSocket queue
    cmd_queue = createQueue();

	// open local serial port
	fprintf(stderr, "Attempting serial connection on %s...\n", dev_name);
	if(!ser_open(dev_name)) {
		fprintf(stderr, "Unable to open serial port %s, available ports follow:\n\n", dev_name);
		ser_enum(print_port);
		return 2;
	}
	if(!ser_config("9600,N,8,1")) {
		ser_close();
		fprintf(stderr, "Unable to configure %s baudrate and format\n", dev_name);
		return 2;
	}
	device_connected = true;
	can_send = false;
	ser_write("S\n", 2);
	fprintf(stderr, "Serial connected.\n");

		
	// outer loop just tries to connect forever (until Ctrl-C)
	while(1) {

		// initialize - open websocket and open device
		snprintf(host, sizeof(host), spjs_host);
		snprintf(port, sizeof(port), spjs_port);
		fprintf(stderr, "Attempting websocket connection to %s:%s...\n", host, port);
		while(websocket_init( host, port )) {
			fprintf(stderr, "Attempting websocket connection to %s:%s...\n", host, port);
			Sleep(1000);
		}
		tinyg_connected = true;
		fprintf(stderr, "Websocket connected.\n");

		ft.QuadPart = 0;
		timer = CreateWaitableTimer(NULL, FALSE, NULL);
		SetWaitableTimer(timer, &ft, 2, NULL, NULL, 0);

		menu_setup();
		
        // The main loop we operate in, will exit on any error
        while(WaitForSingleObject(timer, 5000) == WAIT_OBJECT_0 && !quit) {
            // if we have lost connection to the websocket,
			// then break the loop and start over.
            if(!tinyg_connected) {
                reset_connections();
                break;
            }
			
			// Read from websocket
			while(!!(test = websocket_read())) {
				// JSON message received from SPJS
				//printf("%s\n", test);
				free(test);
			}

			// Communication device => application(dro_menu.c)
			char q;
			while(ser_read(&q, 1)) {
				if((q > 0x1F && q < 0x7F) || q == 0x0A) {
					if(q == 0x0A) {
						if(ser_ix == 0) {
							// Empty packet is "ready for command"
							can_send = true;
						} else if(ser_ix < sizeof(ser)) {
							// Command ready
							switch(ser[0]) {
								case 'V': // velocity drop notification
									if(ser_ix != 2) break;
									switch(ser[1]) {
										case 'X': on_velocity_drop(0); break;
										case 'Y': on_velocity_drop(1); break;
										case 'Z': on_velocity_drop(2); break;
									}
									break;
								case 'S': // status report
									if(ser_ix != 3) break;
									//ser[3] = 0;
									//printf("status:%s\n",ser);
									on_state_change(read_hex_be(&ser[1], 2));
									break;
								case '+': case '-': // button event
									if(ser_ix != 2) break;
									if(ser[0] == '+') on_button_pressed(read_hex_be(&ser[1], 1) - 1);
									if(ser[0] == '-') on_button_release(read_hex_be(&ser[1], 1) - 1);
									break;
								case 'X': case 'Y': case 'Z': // read out
									if((ser_ix % 5) != 0) break;
									for(n = 0; n < ser_ix / 5; n++) {
										switch(ser[n * 5]) {
											case 'X':
												dro[0] = ((double)read_hex_be(&ser[n * 5 + 1], 4)) / 100.0;
												break;
											case 'Y':
												dro[1] = ((double)read_hex_be(&ser[n * 5 + 1], 4)) / 100.0;
												break;
											case 'Z':
												dro[2] = ((double)read_hex_be(&ser[n * 5 + 1], 4)) / 100.0;
												break;
												
										}
									}
									//printf("DRO: X%0.2f Y%0.2f Z%0.2f\n", dro[0], dro[1], dro[2]);
									on_readout(dro[0], dro[1], dro[2]);
									break;
							}
						}
						ser_ix = 0;
					} else if(ser_ix < sizeof(ser)) {
						// data
						ser[ser_ix++] = q;
					}
				}
			}
			
			// Call application handler
			menu_loop();
			
			// Communication application(dro_menu.c) => device
			// This is a priority list, as only one command can be sent here
			if(can_send) {
				can_send = false;
				if(velocity_drop != previous_drop) {
					previous_drop = velocity_drop;
					sprintf(cmd, "V%X%X\n", velocity_drop & 0xF, velocity_drop >> 4); // "V00\n" == "v\n"
					ser_write(cmd, strlen(cmd));
				} else if(device_leds != previous_leds) {
					previous_leds = device_leds;
					sprintf(cmd, "L%X%X\n", device_leds & 0xF, device_leds >> 4);
					ser_write(cmd, strlen(cmd));
				} else if(strcmp(device_text, previous_text)) {
					//printf("%s\n%s", device_text, previous_text);
					strcpy(previous_text, device_text);
					
					sprintf(cmd, "T%s\n", device_text);
					ser_write(cmd, strlen(cmd));
				} else if(memcmp(device_menu, previous_menu, 12)) {
					memcpy(previous_menu, device_menu, 12);
					sprintf(cmd, "B%s\n", device_menu);
					ser_write(cmd, strlen(cmd));
				} else if(send_home) {
					if(send_home == 2)
						ser_write("H\n", 2);
					else
						ser_write("h\n", 2);
					send_home = 0;
				} else if(send_coord) {
					if(send_coord == 2)
						ser_write("M\n", 2);
					else
						ser_write("m\n", 2);
					send_coord = 0;
				} else if(send_zero) {
					ser_write("Z\n", 2);
					send_zero = 0;
				} else if(send_dro) {
					ser_write("R\n", 2);
					send_dro = 0;
				} else can_send = true;
			}

        }

		CancelWaitableTimer(timer);
		CloseHandle(timer);

        Sleep(1000);
    }
}