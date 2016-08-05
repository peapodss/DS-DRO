// Modified version of the Arduino TV-out library.
//
// Optimized for frequent line handler swapping to facilitate adding in gaps (regions of blank lines)
// so that the interface takes up the whole screen with a smaller screen buffer (saves RAM).
//
// Modified delay_frames behaviour (now named wait_for_vsync).
//
// Many TV-out features broken. This is application specific code.

/*
 Copyright (c) 2010 Myles Metzer

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
*/

/* A note about how Color is defined for this version of TVout
 *
 * Where ever choosing a color is mentioned the following are true:
 * 	BLACK	=0
 *	WHITE	=1
 *	INVERT	=2
 *	All others will be ignored.
*/

#include "Composite.h"


#include <avr/interrupt.h>
#include <avr/io.h>

TVout_vid display;
void (*hbi_hook)() = &empty;
void (*vbi_hook)() = &empty;

// sound properties
volatile long remainingToneVsyncs;

static int renderLine;
static unsigned char hbi_count, hbi_state, hbi_active;
static volatile bool waiting;


void empty() {}

// Added two intermediate blanking regions
#define BLANK1_START  148
#define BLANK1_LENGTH  22
#define BLANK1_END       (BLANK1_START + BLANK1_LENGTH)
#define BLANK2_START  252
#define BLANK2_LENGTH  22
#define BLANK2_END       (BLANK2_START + BLANK2_LENGTH)


void render_setup(uint8_t x, uint8_t y, uint8_t *scrnptr) {

	display.screen = scrnptr;
	display.hres = x;
	display.vres = y;
	display.frames = 0;
	
	display.vscale_const = _PAL_LINE_DISPLAY/display.vres - 1;
	display.vscale = display.vscale_const;

	DDR_VID |= _BV(VID_PIN);
	DDR_SYNC |= _BV(SYNC_PIN);
	PORT_VID &= ~_BV(VID_PIN);
	PORT_SYNC |= _BV(SYNC_PIN);
	DDR_SND |= _BV(SND_PIN);	// for tone generation.
	
	// inverted fast pwm mode on timer 1
	TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM11);
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
	
	display.start_render = _PAL_LINE_MID - ((display.vres * (display.vscale_const+1))/2);
  display.end_render = (int)((display.start_render + (display.vres*(display.vscale_const+1))))-1+BLANK1_LENGTH+BLANK2_LENGTH;
	display.output_delay = _PAL_CYCLES_OUTPUT_START;
	display.vsync_end = _PAL_LINE_STOP_VSYNC;
	display.lines_frame = _PAL_LINE_FRAME;
	ICR1 = _PAL_CYCLES_SCANLINE;
	OCR1A = _CYCLES_HORZ_SYNC;

	TIMSK1 = _BV(TOIE1);
	sei();
}


// render a line
ISR(TIMER1_OVF_vect) {
	hbi_hook();
  
  if(hbi_active) {
    // == ACTIVE LINE ==
    wait_until(display.output_delay);
    render_line6c();
    if(!display.vscale) {
      display.vscale = display.vscale_const;
      renderLine += display.hres;
    } else display.vscale--;
    // =================
  }
  display.scanLine++;

  if(hbi_count == 0) {
    switch(hbi_state) {
      case 0: // Start of FRAME
        // == VSYNC ENABLE ==
        OCR1A = _CYCLES_VIRT_SYNC;
        display.frames++;
        PORTB &= ~(_BV(SND_PIN));
        // ==================
        display.scanLine = 0;
        hbi_count = display.vsync_end + 1;
        hbi_state = 1;
        break;
      case 1: // Start of FIELD
        // == VSYNC DISABLE ==
        OCR1A = _CYCLES_HORZ_SYNC;
        // ===================
        hbi_count = display.start_render - display.vsync_end;
        hbi_state = 2;
        break;
      /*
      case 2: // Start of ACTIVE part of field
        hbi_active = true;
        renderLine = 0;
        display.vscale = display.vscale_const;
        hbi_count = display.end_render - display.start_render;
        hbi_state = 3;
        break;
      case 3: // End of ACTIVE part of field
        hbi_active = false; 
        hbi_count = display.lines_frame - display.end_render;
        hbi_state = 0;
        break;
      */
      case 2: // Start of ACTIVE part of field
        hbi_active = true;
        renderLine = 0;
        display.vscale = display.vscale_const;
        hbi_count = BLANK1_START - display.start_render;
        hbi_state = 3;
        break;
      case 3: // First blank region
        hbi_active = false; 
        hbi_count = BLANK1_END - BLANK1_START;
        hbi_state = 4;
        break;
      case 4: // End first blank region
        hbi_active = true;
        hbi_count = BLANK2_START - BLANK1_END;
        hbi_state = 5;
        break;
      case 5: // Second blank region
        hbi_active = false; 
        hbi_count = BLANK2_END - BLANK2_START;
        hbi_state = 6;
        break;
      case 6: // End second blank region
        hbi_active = true;
        hbi_count = display.end_render - BLANK2_END;
        hbi_state = 7;
        break;
      case 7: // End of ACTIVE part of field
        waiting = false;
        hbi_active = false; 
        hbi_count = display.lines_frame - display.end_render;
        hbi_state = 0;
        break;
    }
  }
  hbi_count--;
}

// delay macros
__asm__ __volatile__ (
  // delay 1 clock cycle.
  ".macro delay1\n\t"
    "nop\n"
  ".endm\n"
  
  // delay 2 clock cycles
  ".macro delay2\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
  
  // delay 3 clock cyles
  ".macro delay3\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
  
  // delay 4 clock cylces
  ".macro delay4\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
  
  // delay 5 clock cylces
  ".macro delay5\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
  
  // delay 6 clock cylces
  ".macro delay6\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
  
  // delay 7 clock cylces
  ".macro delay7\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
  
  // delay 8 clock cylces
  ".macro delay8\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
  
  // delay 9 clock cylces
  ".macro delay9\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
  
  // delay 10 clock cylces
  ".macro delay10\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
); // end of delay macros

// common output macros, specific output macros at top of file
__asm__ __volatile__ (
  
  // save port 16 and clear the video bit
  ".macro svprt p\n\t"
    "in   r16,\\p\n\t"
    ANDI_HWS
  ".endm\n"
  
  // ouput 1 bit port safe
  ".macro o1bs p\n\t"
    BLD_HWS
    "out  \\p,r16\n"
  ".endm\n"
); // end of output macros

static void inline wait_until(uint8_t time) {
	__asm__ __volatile__ (
			"subi	%[time], 10\n"
			"sub	%[time], %[tcnt1l]\n\t"
		"100:\n\t"
			"subi	%[time], 3\n\t"
			"brcc	100b\n\t"
			"subi	%[time], 0-3\n\t"
			"breq	101f\n\t"
			"dec	%[time]\n\t"
			"breq	102f\n\t"
			"rjmp	102f\n"
		"101:\n\t"
			"nop\n" 
		"102:\n"
		:
		: [time] "a" (time),
		[tcnt1l] "a" (TCNT1L)
	);
}

void render_line6c() {
	__asm__ __volatile__ (
		"ADD	r26,r28\n\t"
		"ADC	r27,r29\n\t"
		//save PORTB
		"svprt	%[port]\n\t"
		
		"rjmp	enter6\n"
	"loop6:\n\t"
		"bst	__tmp_reg__,0\n\t"			//8
		"o1bs	%[port]\n"
	"enter6:\n\t"
		"LD		__tmp_reg__,X+\n\t"			//1
		"delay1\n\t"
		"bst	__tmp_reg__,7\n\t"
		"o1bs	%[port]\n\t"
		"delay3\n\t"						//2
		"bst	__tmp_reg__,6\n\t"
		"o1bs	%[port]\n\t"
		"delay3\n\t"						//3
		"bst	__tmp_reg__,5\n\t"
		"o1bs	%[port]\n\t"
		"delay3\n\t"						//4
		"bst	__tmp_reg__,4\n\t"
		"o1bs	%[port]\n\t"
		"delay3\n\t"						//5
		"bst	__tmp_reg__,3\n\t"
		"o1bs	%[port]\n\t"
		"delay3\n\t"						//6
		"bst	__tmp_reg__,2\n\t"
		"o1bs	%[port]\n\t"
		"delay3\n\t"						//7
		"bst	__tmp_reg__,1\n\t"
		"o1bs	%[port]\n\t"
		"dec	%[hres]\n\t"
		"brne	loop6\n\t"					//go too loopsix
		"delay2\n\t"
		"bst	__tmp_reg__,0\n\t"			//8
		"o1bs	%[port]\n"
		
		"svprt	%[port]\n\t"
		BST_HWS
		"o1bs	%[port]\n\t"
		:
		: [port] "i" (_SFR_IO_ADDR(PORT_VID)),
		"x" (display.screen),
		"y" (renderLine),
		[hres] "d" (display.hres)
		: "r16" // try to remove this clobber later...
	);
}

/* Stop video render and free the used memory.
 */
 void TVout::end() {
  TIMSK1 = 0;
}
 
char TVout::begin() {
		
  const uint8_t x = TV_WIDTH/8;
  const uint8_t y = TV_HEIGHT;

  cursor_x = 0;
  cursor_y = 0;
  
  render_setup(x,y,screen);
  clear_screen();
  return 0;
   
} // end of begin


/* Fill the screen with some color.
 *
 * Arguments:
 *	color:
 *		The color to fill the screen with.
 *		(see color note at the top of this file)
*/
void TVout::fill(uint8_t color) {
	switch(color) {
		case BLACK:
			cursor_x = 0;
			cursor_y = 0;
			for (int i = 0; i < (display.hres)*display.vres; i++)
				display.screen[i] = 0;
			break;
		case WHITE:
			cursor_x = 0;
			cursor_y = 0;
			for (int i = 0; i < (display.hres)*display.vres; i++)
				display.screen[i] = 0xFF;
			break;
		case INVERT:
			for (int i = 0; i < display.hres*display.vres; i++)
				display.screen[i] = ~display.screen[i];
			break;
	}
} // end of fill


/* Gets the Horizontal resolution of the screen
 *
 * Returns: 
 *	The horizonal resolution.
*/
unsigned char TVout::hres() {
	return display.hres*8;
} // end of hres


/* Gets the Vertical resolution of the screen
 *
 * Returns:
 *	The vertical resolution
*/
unsigned char TVout::vres() {
	return display.vres;
} // end of vres


/* Return the number of characters that will fit on a line
 *
 * Returns:
 *	The number of characters that will fit on a text line starting from x=0.
 *	Will return -1 for dynamic width fonts as this cannot be determined.
*/
char TVout::char_line() {
	return ((display.hres*8)/pgm_read_byte(font));
} // end of char_line


/* delay for x ms
 * The resolution is 16ms for NTSC and 20ms for PAL
 *
 * Arguments:
 *	x:
 *		The number of ms this function should consume.
*/
void TVout::delay(unsigned int x) {
	unsigned long time = millis() + x;
	while(millis() < time);
} // end of delay


/* Delay for x frames, exits at the end of the last display line.
 * wait_for_vsync() is useful prior to drawing so there is little/no flicker.
 *
 * Arguments:
 *	x:
 *		The number of frames to delay for.
 */
void TVout::wait_for_vsync() {
	waiting = true;
  while(waiting);
} // end of delay_frame


/* Get the time in ms since begin was called.
 * The resolution is 16ms for NTSC and 20ms for PAL
 *
 * Returns:
 *	The time in ms since video generation has started.
*/
unsigned long TVout::millis() {
		return display.frames * _PAL_TIME_SCANLINE * _PAL_LINE_FRAME / 1000;
} // end of millis


/* force the number of times to display each line.
 *
 * Arguments:
 *	sfactor:
 *		The scale number of times to repeate each line.
 */
void TVout::force_vscale(char sfactor) {
	wait_for_vsync();
	display.vscale_const = sfactor - 1;
	display.vscale = sfactor - 1;
}


/* force the output start time of a scanline in micro seconds.
 *
 * Arguments:
 *	time:
 *		The new output start time in micro seconds.
 */
void TVout::force_outstart(uint8_t time) {
	wait_for_vsync();
	display.output_delay = ((time * _CYCLES_PER_US) - 1);
}


/* force the start line for active video
 *
 * Arguments:
 *	line:
 *		The new active video output start line
 */
void TVout::force_linestart(uint8_t line) {
	wait_for_vsync();
	display.start_render = line;
}


/* Set the color of a pixel
 * 
 * Arguments:
 *	x:
 *		The x coordinate of the pixel.
 *	y:
 *		The y coordinate of the pixel.
 *	c:
 *		The color of the pixel
 *		(see color note at the top of this file)
 */
void TVout::set_pixel(uint8_t x, uint8_t y, char c) {
	if (x >= display.hres*8 || y >= display.vres)
		return;
	sp(x,y,c);
} // end of set_pixel


/* get the color of the pixel at x,y
 * 
 * Arguments:
 *	x:
 *		The x coordinate of the pixel.
 *	y:
 *		The y coordinate of the pixel.
 *
 * Returns:
 *	The color of the pixel.
 *	(see color note at the top of this file)
 *
 * Thank you gijs on the arduino.cc forum for the non obviouse fix.
*/
unsigned char TVout::get_pixel(uint8_t x, uint8_t y) {
	if (x >= display.hres*8 || y >= display.vres)
		return 0;
	if (display.screen[x/8+y*display.hres] & (0x80 >>(x&7)))
		return 1;
	return 0;
} // end of get_pixel


/* Draw a line from one point to another
 *
 * Arguments:
 *	x0:
 *		The x coordinate of point 0.
 *	y0:
 *		The y coordinate of point 0.
 *	x1:
 *		The x coordinate of point 1.
 *	y1:
 *		The y coordinate of point 1.
 *	c:
 *		The color of the line.
 *		(see color note at the top of this file)
 */
void TVout::draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, char c) {

	if (x0 > display.hres*8 || y0 > display.vres || x1 > display.hres*8 || y1 > display.vres)
		return;
	if (x0 == x1)
		draw_column(x0,y0,y1,c);
	else if (y0 == y1)
		draw_row(y0,x0,x1,c);
	else {
		int e;
		signed int dx,dy,j, temp;
		signed char s1,s2, xchange;
		signed int x,y;

		x = x0;
		y = y0;
	
		//take absolute value
		if (x1 < x0) {
			dx = x0 - x1;
			s1 = -1;
		}
		else if (x1 == x0) {
			dx = 0;
			s1 = 0;
		}
		else {
			dx = x1 - x0;
			s1 = 1;
		}

		if (y1 < y0) {
			dy = y0 - y1;
			s2 = -1;
		}
		else if (y1 == y0) {
			dy = 0;
			s2 = 0;
		}
		else {
			dy = y1 - y0;
			s2 = 1;
		}

		xchange = 0;   

		if (dy>dx) {
			temp = dx;
			dx = dy;
			dy = temp;
			xchange = 1;
		} 

		e = ((int)dy<<1) - dx;  
	 
		for (j=0; j<=dx; j++) {
			sp(x,y,c);
		 
			if (e>=0) {
				if (xchange==1) x = x + s1;
				else y = y + s2;
				e = e - ((int)dx<<1);
			}
			if (xchange==1)
				y = y + s2;
			else
				x = x + s1;
			e = e + ((int)dy<<1);
		}
	}
} // end of draw_line


/* Fill a row from one point to another
 *
 * Argument:
 *	line:
 *		The row that fill will be performed on.
 *	x0:
 *		edge 0 of the fill.
 *	x1:
 *		edge 1 of the fill.
 *	c:
 *		the color of the fill.
 *		(see color note at the top of this file)
*/
void TVout::draw_row(uint8_t line, uint16_t x0, uint16_t x1, uint8_t c) {
	uint8_t lbit, rbit;
	
	if (x0 == x1)
		set_pixel(x0,line,c);
	else {
		if (x0 > x1) {
			lbit = x0;
			x0 = x1;
			x1 = lbit;
		}
		lbit = 0xff >> (x0&7);
		x0 = x0/8 + display.hres*line;
		rbit = ~(0xff >> (x1&7));
		x1 = x1/8 + display.hres*line;
		if (x0 == x1) {
			lbit = lbit & rbit;
			rbit = 0;
		}
		if (c == WHITE) {
			screen[x0++] |= lbit;
			while (x0 < x1)
				screen[x0++] = 0xff;
			screen[x0] |= rbit;
		}
		else if (c == BLACK) {
			screen[x0++] &= ~lbit;
			while (x0 < x1)
				screen[x0++] = 0;
			screen[x0] &= ~rbit;
		}
		else if (c == INVERT) {
			screen[x0++] ^= lbit;
			while (x0 < x1)
				screen[x0++] ^= 0xff;
			screen[x0] ^= rbit;
		}
	}
} // end of draw_row


/* Fill a column from one point to another
 *
 * Argument:
 *	row:
 *		The row that fill will be performed on.
 *	y0:
 *		edge 0 of the fill.
 *	y1:
 *		edge 1 of the fill.
 *	c:
 *		the color of the fill.
 *		(see color note at the top of this file)
*/
void TVout::draw_column(uint8_t row, uint16_t y0, uint16_t y1, uint8_t c) {

	unsigned char bit;
	int byte;
	
	if (y0 == y1)
		set_pixel(row,y0,c);
	else {
		if (y1 < y0) {
			bit = y0;
			y0 = y1;
			y1 = bit;
		}
		bit = 0x80 >> (row&7);
		byte = row/8 + y0*display.hres;
		if (c == WHITE) {
			while ( y0 <= y1) {
				screen[byte] |= bit;
				byte += display.hres;
				y0++;
			}
		}
		else if (c == BLACK) {
			while ( y0 <= y1) {
				screen[byte] &= ~bit;
				byte += display.hres;
				y0++;
			}
		}
		else if (c == INVERT) {
			while ( y0 <= y1) {
				screen[byte] ^= bit;
				byte += display.hres;
				y0++;
			}
		}
	}
}


/* draw a rectangle at x,y with a specified width and height
 * 
 * Arguments:
 *	x0:
 *		The x coordinate of upper left corner of the rectangle.
 *	y0:
 *		The y coordinate of upper left corner of the rectangle.
 *	w:
 *		The widht of the rectangle.
 *	h:
 *		The height of the rectangle.
 *	c:
 *		The color of the rectangle.
 *		(see color note at the top of this file)
 *	fc:
 *		The fill color of the rectangle.
 *		(see color note at the top of this file)
 *		default =-1 (no fill)
*/
void TVout::draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc) {
	
	if (fc != -1) {
		for (unsigned char i = y0; i < y0+h; i++)
			draw_row(i,x0,x0+w,fc);
	}
	draw_line(x0,y0,x0+w,y0,c);
	draw_line(x0,y0,x0,y0+h,c);
	draw_line(x0+w,y0,x0+w,y0+h,c);
	draw_line(x0,y0+h,x0+w,y0+h,c);
} // end of draw_rect


/* draw a circle given a coordinate x,y and radius both filled and non filled.
 *
 * Arguments:
 * 	x0:
 *		The x coordinate of the center of the circle.
 *	y0:
 *		The y coordinate of the center of the circle.
 *	radius:
 *		The radius of the circle.
 *	c:
 *		The color of the circle.
 *		(see color note at the top of this file)
 *	fc:
 *		The color to fill the circle.
 *		(see color note at the top of this file)
 *		defualt  =-1 (do not fill)
 */
void TVout::draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, char c, char fc) {

	int f = 1 - radius;
	int ddF_x = 1;
	int	ddF_y = -2 * radius;
	int x = 0;
	int y = radius;
	uint8_t pyy = y,pyx = x;
	
	
	//there is a fill color
	if (fc != -1)
		draw_row(y0,x0-radius,x0+radius,fc);
	
	sp(x0, y0 + radius,c);
	sp(x0, y0 - radius,c);
	sp(x0 + radius, y0,c);
	sp(x0 - radius, y0,c);
	
	while(x < y) {
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		//there is a fill color
		if (fc != -1) {
			//prevent double draws on the same rows
			if (pyy != y) {
				draw_row(y0+y,x0-x,x0+x,fc);
				draw_row(y0-y,x0-x,x0+x,fc);
			}
			if (pyx != x && x != y) {
				draw_row(y0+x,x0-y,x0+y,fc);
				draw_row(y0-x,x0-y,x0+y,fc);
			}
			pyy = y;
			pyx = x;
		}
		sp(x0 + x, y0 + y,c);
		sp(x0 - x, y0 + y,c);
		sp(x0 + x, y0 - y,c);
		sp(x0 - x, y0 - y,c);
		sp(x0 + y, y0 + x,c);
		sp(x0 - y, y0 + x,c);
		sp(x0 + y, y0 - x,c);
		sp(x0 - y, y0 - x,c);
	}
} // end of draw_circle


/* place a bitmap at x,y where the bitmap is defined as {width,height,imagedata....}
 *
 * Arguments:
 *	x:
 *		The x coordinate of the upper left corner.
 *	y:
 *		The y coordinate of the upper left corner.
 *	bmp:
 *		The bitmap data to print.
 *	i:
 *		The offset into the image data to start at.  This is mainly used for fonts.
 *		default	=0
 *	width:
 *		Override the bitmap width. This is mainly used for fonts.
 *		default =0 (do not override)
 *	height:
 *		Override the bitmap height. This is mainly used for fonts.
 *		default	=0 (do not override)
*/
void TVout::bitmap(uint8_t x, uint8_t y, const unsigned char * bmp,
				   uint16_t i, uint8_t width, uint8_t lines) {

	uint8_t temp, lshift, rshift, save, xtra;
	uint16_t si = 0;
	
	rshift = x&7;
	lshift = 8-rshift;
	if (width == 0) {
		width = pgm_read_byte((uint32_t)(bmp) + i);
		i++;
	}
	if (lines == 0) {
		lines = pgm_read_byte((uint32_t)(bmp) + i);
		i++;
	}
		
	if (width&7) {
		xtra = width&7;
		width = width/8;
		width++;
	}
	else {
		xtra = 8;
		width = width/8;
	}
	
	for (uint8_t l = 0; l < lines; l++) {
		si = (y + l)*display.hres + x/8;
		if (width == 1)
			temp = 0xff >> rshift + xtra;
		else
			temp = 0;
		save = screen[si];
		screen[si] &= ((0xff << lshift) | temp);
		temp = pgm_read_byte((uint32_t)(bmp) + i++);
		screen[si++] |= temp >> rshift;
		for ( uint16_t b = i + width-1; i < b; i++) {
			save = screen[si];
			screen[si] = temp << lshift;
			temp = pgm_read_byte((uint32_t)(bmp) + i);
			screen[si++] |= temp >> rshift;
		}
		if (rshift + xtra < 8)
			screen[si-1] |= (save & (0xff >> rshift + xtra));	//test me!!!
		if (rshift + xtra - 8 > 0)
			screen[si] &= (0xff >> rshift + xtra - 8);
		screen[si] |= temp << lshift;
	}
} // end of bitmap


/* shift the pixel buffer in any direction
 * This function will shift the screen in a direction by any distance.
 *
 * Arguments:
 *	distance:
 *		The distance to shift the screen
 *	direction:
 *		The direction to shift the screen the direction and the integer values:
 *		UP		=0
 *		DOWN	=1
 *		LEFT	=2
 *		RIGHT	=3
*/
void TVout::shift(uint8_t distance, uint8_t direction) {
	uint8_t * src;
	uint8_t * dst;
	uint8_t * end;
	uint8_t shift;
	uint8_t tmp;
	switch(direction) {
		case UP:
			dst = display.screen;
			src = display.screen + distance*display.hres;
			end = display.screen + display.vres*display.hres;
				
			while (src <= end) {
				*dst = *src;
				*src = 0;
				dst++;
				src++;
			}
			break;
		case DOWN:
			dst = display.screen + display.vres*display.hres;
			src = dst - distance*display.hres;
			end = display.screen;
				
			while (src >= end) {
				*dst = *src;
				*src = 0;
				dst--;
				src--;
			}
			break;
		case LEFT:
			shift = distance & 7;
			
			for (uint8_t line = 0; line < display.vres; line++) {
				dst = display.screen + display.hres*line;
				src = dst + distance/8;
				end = dst + display.hres-2;
				while (src <= end) {
					tmp = 0;
					tmp = *src << shift;
					*src = 0;
					src++;
					tmp |= *src >> (8 - shift);
					*dst = tmp;
					dst++;
				}
				tmp = 0;
				tmp = *src << shift;
				*src = 0;
				*dst = tmp;
			}
			break;
		case RIGHT:
			shift = distance & 7;
			
			for (uint8_t line = 0; line < display.vres; line++) {
				dst = display.screen + display.hres-1 + display.hres*line;
				src = dst - distance/8;
				end = dst - display.hres+2;
				while (src >= end) {
					tmp = 0;
					tmp = *src >> shift;
					*src = 0;
					src--;
					tmp |= *src << (8 - shift);
					*dst = tmp;
					dst--;
				}
				tmp = 0;
				tmp = *src >> shift;
				*src = 0;
				*dst = tmp;
			}
			break;
	}
} // end of shift


/* Inline version of set_pixel that does not perform a bounds check
 * This function will be replaced by a macro.
*/
static void inline sp(uint8_t x, uint8_t y, char c) {
	if (c==1)
		display.screen[(x/8) + (y*display.hres)] |= 0x80 >> (x&7);
	else if (c==0)
		display.screen[(x/8) + (y*display.hres)] &= ~0x80 >> (x&7);
	else
		display.screen[(x/8) + (y*display.hres)] ^= 0x80 >> (x&7);
} // end of sp


/* set the vertical blank function call
 * The function passed to this function will be called one per frame. The function should be quickish.
 *
 * Arguments:
 *	func:
 *		The function to call.
 */
void TVout::set_vbi_hook(void (*func)()) {
	vbi_hook = func;
} // end of set_vbi_hook


/* set the horizonal blank function call
 * This function passed to this function will be called one per scan line.
 * The function MUST be VERY FAST(~2us max).
 *
 * Arguments:
 *	funct:
 *		The function to call.
 */
void TVout::set_hbi_hook(void (*func)()) {
	hbi_hook = func;
} // end of set_bhi_hook


/* Simple tone generation
 *
 * Arguments:
 *	frequency:
 *		the frequency of the tone
 * courtesy of adamwwolf
 */
void TVout::tone(unsigned int frequency) {
	tone(frequency, 0);
} // end of tone


/* Simple tone generation
 *
 * Arguments:
 *	frequency:
 *		the frequency of the tone
 *	duration_ms:
 *		The duration to play the tone in ms
 * courtesy of adamwwolf
 */
void TVout::tone(unsigned int frequency, unsigned long duration_ms) {

	if (frequency == 0)
		return;

#define TIMER 2
	//this is init code
	TCCR2A = 0;
	TCCR2B = 0;
	TCCR2A |= _BV(WGM21);
	TCCR2B |= _BV(CS20);
	//end init code

	//most of this is taken from Tone.cpp from Arduino
	uint8_t prescalarbits = 0b001;
	uint32_t ocr = 0;
  

    DDR_SND |= _BV(SND_PIN); //set pb3 (digital pin 11) to output

    //we are using an 8 bit timer, scan through prescalars to find the best fit
	ocr = F_CPU / frequency / 2 - 1;
    prescalarbits = 0b001;  // ck/1: same for both timers
    if (ocr > 255) {
        ocr = F_CPU / frequency / 2 / 8 - 1;
        prescalarbits = 0b010;  // ck/8: same for both timers

        if (ocr > 255) {
			ocr = F_CPU / frequency / 2 / 32 - 1;
			prescalarbits = 0b011;
        }

        if (ocr > 255) {
			ocr = F_CPU / frequency / 2 / 64 - 1;
			prescalarbits = TIMER == 0 ? 0b011 : 0b100;
			if (ocr > 255) {
				ocr = F_CPU / frequency / 2 / 128 - 1;
				prescalarbits = 0b101;
			}

			if (ocr > 255) {
				ocr = F_CPU / frequency / 2 / 256 - 1;
				prescalarbits = TIMER == 0 ? 0b100 : 0b110;
				if (ocr > 255) {
					// can't do any better than /1024
					ocr = F_CPU / frequency / 2 / 1024 - 1;
					prescalarbits = TIMER == 0 ? 0b101 : 0b111;
				}
			}
        }
    }
    TCCR2B = prescalarbits;

	if (duration_ms > 0)
		remainingToneVsyncs = duration_ms*60/1000; //60 here represents the framerate
	else
		remainingToneVsyncs = -1;
 
    // Set the OCR for the given timer,
    OCR2A = ocr;
    //set it to toggle the pin by itself
    TCCR2A &= ~(_BV(COM2A1)); //set COM2A1 to 0
    TCCR2A |= _BV(COM2A0);
} // end of tone

/* Stops tone generation
 */
void TVout::noTone() {
	TCCR2B = 0;
	PORT_SND &= ~(_BV(SND_PIN)); //set pin 11 to 0
} // end of noTone

// PRINTING


void TVout::select_font(const unsigned char * f) {
  font = f;
}

/*
 * print an 8x8 char c at x,y
 * x must be a multiple of 8
 */
void TVout::print_char(uint8_t x, uint8_t y, unsigned char c) {

  c -= pgm_read_byte(font+2);
  bitmap(x,y,font,(c*pgm_read_byte(font+1))+3,pgm_read_byte(font),pgm_read_byte(font+1));
}

void TVout::inc_txtline() {
  if (cursor_y >= (display.vres - pgm_read_byte(font+1)))
    shift(pgm_read_byte(font+1),UP);
  else
    cursor_y += pgm_read_byte(font+1);
}

/* default implementation: may be overridden */
void TVout::write(const char *str)
{
  while (*str)
    write(*str++);
}

/* default implementation: may be overridden */
void TVout::write(const uint8_t *buffer, uint8_t size)
{
  while (size--)
    write(*buffer++);
}

void TVout::write(uint8_t c) {
  switch(c) {
    case '\0':      //null
      break;
    case '\n':      //line feed
      cursor_x = 0;
      inc_txtline();
      break;
    case 8:       //backspace
      cursor_x -= pgm_read_byte(font);
      print_char(cursor_x,cursor_y,' ');
      break;
    case 13:      //carriage return !?!?!?!VT!?!??!?!
      cursor_x = 0;
      break;
    case 14:      //form feed new page(clear screen)
      //clear_screen();
      break;
    default:
      if (cursor_x >= (display.hres*8 - pgm_read_byte(font))) {
        cursor_x = 0;
        inc_txtline();
        print_char(cursor_x,cursor_y,c);
      }
      else
        print_char(cursor_x,cursor_y,c);
      cursor_x += pgm_read_byte(font);
  }
}

void TVout::print(const char str[])
{
  write(str);
}

void TVout::print(char c, int base)
{
  print((long) c, base);
}

void TVout::print(unsigned char b, int base)
{
  print((unsigned long) b, base);
}

void TVout::print(int n, int base)
{
  print((long) n, base);
}

void TVout::print(unsigned int n, int base)
{
  print((unsigned long) n, base);
}

void TVout::print(long n, int base)
{
  if (base == 0) {
    write(n);
  } else if (base == 10) {
    if (n < 0) {
      print('-');
      n = -n;
    }
    printNumber(n, 10);
  } else {
    printNumber(n, base);
  }
}

void TVout::print(unsigned long n, int base)
{
  if (base == 0) write(n);
  else printNumber(n, base);
}

void TVout::print(double n, int digits)
{
  printFloat(n, digits);
}

void TVout::println(void)
{
  print('\r');
  print('\n');  
}

void TVout::println(const char c[])
{
  print(c);
  println();
}

void TVout::println(char c, int base)
{
  print(c, base);
  println();
}

void TVout::println(unsigned char b, int base)
{
  print(b, base);
  println();
}

void TVout::println(int n, int base)
{
  print(n, base);
  println();
}

void TVout::println(unsigned int n, int base)
{
  print(n, base);
  println();
}

void TVout::println(long n, int base)
{
  print(n, base);
  println();
}

void TVout::println(unsigned long n, int base)
{
  print(n, base);
  println();
}

void TVout::println(double n, int digits)
{
  print(n, digits);
  println();
}

void TVout::printPGM(const char str[]) {
  char c;
  while ((c = pgm_read_byte(str))) {
    str++;
    write(c);
  }
}

void TVout::printPGM(uint8_t x, uint8_t y, const char str[]) {
  char c;
  cursor_x = x;
  cursor_y = y;
  while ((c = pgm_read_byte(str))) {
    str++;
    write(c);
  }
}

void TVout::set_cursor(uint8_t x, uint8_t y) {
  cursor_x = x;
  cursor_y = y;
}

void TVout::print(uint8_t x, uint8_t y, const char str[]) {
  cursor_x = x;
  cursor_y = y;
  write(str);
  
}
void TVout::print(uint8_t x, uint8_t y, char c, int base) {
  cursor_x = x;
  cursor_y = y;
  print((long) c, base);
}
void TVout::print(uint8_t x, uint8_t y, unsigned char b, int base) {
  cursor_x = x;
  cursor_y = y;
  print((unsigned long) b, base);
}
void TVout::print(uint8_t x, uint8_t y, int n, int base) {
  cursor_x = x;
  cursor_y = y;
  print((long) n, base);
}
void TVout::print(uint8_t x, uint8_t y, unsigned int n, int base) {
  cursor_x = x;
  cursor_y = y;
  print((unsigned long) n, base);
}
void TVout::print(uint8_t x, uint8_t y, long n, int base) {
  cursor_x = x;
  cursor_y = y;
  print(n,base);
}
void TVout::print(uint8_t x, uint8_t y, unsigned long n, int base) {
  cursor_x = x;
  cursor_y = y;
  print(n,base);
}
void TVout::print(uint8_t x, uint8_t y, double n, int digits) {
  cursor_x = x;
  cursor_y = y;
  print(n,digits);
}

void TVout::println(uint8_t x, uint8_t y, const char c[])
{
  cursor_x = x;
  cursor_y = y;
  print(c);
  println();
}

void TVout::println(uint8_t x, uint8_t y, char c, int base)
{
  cursor_x = x;
  cursor_y = y;
  print(c, base);
  println();
}

void TVout::println(uint8_t x, uint8_t y, unsigned char b, int base)
{
  cursor_x = x;
  cursor_y = y;
  print(b, base);
  println();
}

void TVout::println(uint8_t x, uint8_t y, int n, int base)
{
  cursor_x = x;
  cursor_y = y;
  print(n, base);
  println();
}

void TVout::println(uint8_t x, uint8_t y, unsigned int n, int base)
{
  cursor_x = x;
  cursor_y = y;
  print(n, base);
  println();
}

void TVout::println(uint8_t x, uint8_t y, long n, int base)
{
  cursor_x = x;
  cursor_y = y;
  print(n, base);
  println();
}

void TVout::println(uint8_t x, uint8_t y, unsigned long n, int base)
{
  cursor_x = x;
  cursor_y = y;
  print(n, base);
  println();
}

void TVout::println(uint8_t x, uint8_t y, double n, int digits)
{
  cursor_x = x;
  cursor_y = y;
  print(n, digits);
  println();
}

void TVout::printNumber(unsigned long n, uint8_t base)
{
  unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
  unsigned long i = 0;

  if (n == 0) {
    print('0');
    return;
  } 

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
    print((char) (buf[i - 1] < 10 ?
      '0' + buf[i - 1] :
      'A' + buf[i - 1] - 10));
}

void TVout::printFloat(double number, uint8_t digits) 
{ 
  // Handle negative numbers
  if (number < 0.0)
  {
     print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    print(toPrint);
    remainder -= toPrint; 
  } 
}
