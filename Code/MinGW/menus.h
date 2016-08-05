#ifndef __MENUS_H__
#define __MENUS_H__
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
void menuClear();
void menuRefresh();
void menuSet(void (*handler)());
void (*menuGet())();
bool menuIs(void (*handler)());
void menuEvent(unsigned char id);
void menuSetItem(unsigned char ix, char *text, void (*fPtr)());
#endif//__MENUS_H__