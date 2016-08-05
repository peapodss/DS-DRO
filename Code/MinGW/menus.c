#include "menus.h"
#include "bridge.h"

// Hardkey menu
static void (*fMenuItem[7])(); // Event handlers
static void (*fMenu)();        // Menu handler

// Hardkey menu: Clear all menu items
void menuClear() {
	unsigned char n;
	clear_menu();
	for(n = 0; n < 7; n++) fMenuItem[n] = NULL;
}

// Hardkey menu: Refresh on state change
void menuRefresh() {
	if(fMenu) fMenu();
}

// Hardkey menu: Set menu handler
bool menuIs(void (*handler)()) {
	return handler == fMenu;
}


// Hardkey menu: Set menu handler
void menuSet(void (*handler)()) {
	if(!menuIs(handler)) {
		fMenu = handler;
		menuRefresh();
	}
}

void(*menuGet())() {
	return fMenu;
}

// Hardkey menu: Call event handler
void menuEvent(unsigned char id) {
	if(fMenuItem[id]) fMenuItem[id]();
}

// Hardkey menu: Set menu item
void menuSetItem(unsigned char ix, char *text, void (*fPtr)()) {
  fMenuItem[ix] = fPtr;
  if(fPtr) {
    set_menu_item(ix, text);
  } else {
	clear_menu_item(ix);
  }
}

// called when user presses a button
void on_button_pressed(uint8_t button) {
//	fprintf(stderr, "button %u +pressed\n", button);
	menuEvent(button);
}
