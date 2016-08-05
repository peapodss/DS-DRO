#ifndef __DRO_MENU_H__
#define __DRO_MENU_H__

#include <stdint.h>
#include <stdbool.h>

void on_button_release(uint8_t button);
void on_button_pressed(uint8_t button);
void on_state_change(uint8_t state);
void on_readout(double, double, double);
void on_velocity_drop(uint8_t axis);
void menu_setup();
void menu_loop();
				
#endif//__DRO_MENU_H__
