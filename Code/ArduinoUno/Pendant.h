// Uses MCP23017 to control a pendant with 7 buttons and 7 leds

#define MCP23017_ADDRESS  0x27

#define MCP23017_IODIRA   0x00
#define MCP23017_IODIRB   0x01
#define MCP23017_IPOLA    0x02
#define MCP23017_IPOLB    0x03
#define MCP23017_GPINTENA 0x04
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALA  0x06
#define MCP23017_DEFVALB  0x07
#define MCP23017_INTCONA  0x08
#define MCP23017_INTCONB  0x09
#define MCP23017_IOCONA   0x0A // aliases IOCONB
#define MCP23017_IOCONB   0x0B // aliases IOCONA
#define MCP23017_GPPUA    0x0C
#define MCP23017_GPPUB    0x0D
#define MCP23017_INTFA    0x0E
#define MCP23017_INTFB    0x0F
#define MCP23017_INTCAPA  0x10
#define MCP23017_INTCAPB  0x11
#define MCP23017_GPIOA    0x12
#define MCP23017_GPIOB    0x13
#define MCP23017_OLATA    0x14
#define MCP23017_OLATB    0x15

// Write single register to MCP23017
void io_write(byte reg, byte oct) {
  Wire.beginTransmission(MCP23017_ADDRESS);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)oct);
  Wire.endTransmission();
}

// Read single register from MCP23017
byte io_read(byte reg) {
  Wire.beginTransmission(MCP23017_ADDRESS);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom(MCP23017_ADDRESS, 1);
  return Wire.read();
}

// Initialize communication with MCP23017
void io_begin() {
  // I2C preamble (corrects Wire library problem)
  digitalWrite(A5, HIGH); // SCL high (+ set as output if not)
  digitalWrite(A4, HIGH); // SDA high (+ set as output if not)
  delay(1);
  digitalWrite(A4, LOW);  // SDA low (start condition)
  delay(1);
  digitalWrite(A4, HIGH); // SDA high (stop condition)

  // Start I2C communication using Wire
  Wire.begin();

  // Reset the MCP23017
  io_write(0x15, 0x00); // Reset IOCON if BANK = 1 (other addressing scheme)
  
  // Tri-state both banks
  io_write(0x00, 0xFF);
  io_write(0x01, 0xFF);
  
  // Reset all remaining registersc
  for(byte n = 2; n < 0x16; n++) { io_write(n, 0x00); }

  // Set application specific configuration
  io_write(MCP23017_GPIOA,  0xFF); // Port A <= all leds off (high)
  io_write(MCP23017_IODIRA, 0x00); // Port A <= enable outputs
  io_write(MCP23017_GPPUB,  0xFF); // Port B <= enable pull-up on inputs
}

byte ledstore;

// Read buttons
byte io_read() {
  io_write(MCP23017_GPIOA, 0xFE);
  byte buttons = io_read(MCP23017_GPIOB);
  io_write(MCP23017_GPIOA, reverseBits(~ledstore));
  buttons = (buttons | 0b11111000) & ((buttons >> 1) | 0b11000111);
  return ~buttons;
}

// Write LEDs
byte io_write(byte leds) {
  leds = (leds & 0b111) | ((leds & 0b111000) << 1);
  ledstore = leds;
  io_write(MCP23017_GPIOA, reverseBits(~ledstore));
}

// Read buttons, set LEDs connected to MCP23017
byte io_refresh(byte leds) {
  digitalWrite(A4, HIGH); // SDA high (stop condition)
  Wire.begin();
  io_write(MCP23017_GPIOA, 0xFE);
  byte buttons = io_read(MCP23017_GPIOB);
  io_write(MCP23017_GPIOA, 0xFE);
  io_write(MCP23017_GPIOA, reverseBits(~leds));
  return ~buttons;
  digitalWrite(A4, LOW);  // SDA low (start condition)

  // Start I2C communication using Wire
  
}
