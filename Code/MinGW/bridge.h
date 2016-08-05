#ifndef __BRIDGE_H__
#define __BRIDGE_H__
#include <stdint.h>

void send_to_tinyg(char *command);
void set_leds(uint8_t leds);
void clear_menu();
void set_menu(char *menu);
void set_menu_item(uint8_t ix, char *item);
void clear_menu_item(uint8_t ix);
void set_text(char *text);
void monitor_velocity(uint8_t limit);

void request_dro();
void enable_homing();
void disable_homing();
void zero_coords();
void enable_machine_coords();
void disable_machine_coords();

#endif//__BRIDGE_H__
