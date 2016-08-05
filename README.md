# DS-DRO
DIY absolute Digital Read-Out system.

Designed for use with TinyG G2 and will:
* Sets up absolute positioning and soft limits
* Allows jogging from keypad/pendant near machine
* Recover coordinate system from crashes to resume work

Uses cheap soxin digital glass rulers, but will handle any quadrature input + limit sw./zero mark.
Implemented on Arduino Uno with CmodC2 CPLD for the first stage.
Uses cheap composite car monitor to display interface.
Single IC user pendant for control.

This is something I built for myself using only parts lying around.
Using an Arduino Due or other faster processor would remove the need for a CPLD/FPGA.
Still, the project contains some useful code even though it is unlikely you'll be able to use it as-is.
