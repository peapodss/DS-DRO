/* == PRETTY MUCH A COMPLETE CONNECTION DIAGRAM == */

// Platform connections to CPLD
#define Latch  6 // Latch counters (strobe before SPI read)
#define Seek  10 // Allows Z signal from ruler to reset counters

#define SCLK  13 // Serial clock (controlled by SPI library)
#define MISO  12 // Serial data (controlled by SPI library)
#define Clock  8 // 16 MHz clock output on PB0 (enabled by fuses, not code)

// Platform connections to composite output
#define Sync   9 // Video sync (controlled by TVout library)
#define Video  7 // Video signal (controlled by TVout library)
#define Audio    // Audio signal (not used/tested)

// Platform connections to handheld pendant
#define SCL   A5 // I²C clock (controlled by Wire library)
#define SDA   A4 // I²C data (controlled by Wire library)

/* =============================================== */

// Read and write ports really fast (using constant pin number)
#define pinPort( P ) ( ( P ) <= 7 ? &PORTD : ( ( P ) <= 13 ? &PORTB : ( ( P ) <= 19 ? &PORTC : &PORTB ) ) )
#define pinBit( P ) ( ( P ) <= 7 ? ( P ) : ( ( P ) <= 13 ? ( P ) - 8 : ( P ) - 14 ) )
#define fastWrite( P, V ) bitWrite( *pinPort( P ), pinBit( P ), ( V ) )
#define fastRead( P ) bitRead( *pinPort( P ), pinBit( P ) )

// Hexadecimals
const char PROGMEM hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

// Reverse all bits in a byte (for CPU without barrel shifting or bit-reversal)
byte reverseBits(byte x) {
  byte q = x & 1;
  x >>= 1; q <<= 1; q |= x & 1;
  x >>= 1; q <<= 1; q |= x & 1;
  x >>= 1; q <<= 1; q |= x & 1;
  x >>= 1; q <<= 1; q |= x & 1;
  x >>= 1; q <<= 1; q |= x & 1;
  x >>= 1; q <<= 1; q |= x & 1;
  x >>= 1; q <<= 1; q |= x & 1;
  return q;
}

// Write unsigned long "dec" to ascii decimal string "o[]" of length "len", right aligned
void unsout(char o[], unsigned int dec, byte len) {
  o[len] = 0;
  o[--len] = '0' + (dec % 10); dec /= 10;
  while(len != 0) {
    o[--len] = dec == 0 ? ' ' : '0' + (dec % 10); dec /= 10;
  }
}

// Write position "dec" to ascii decimal string "o[]" in form "0.00", length 7, right aligned
void posout(char o[], long dec) {
  byte n;
  char sign = dec < 0 ? '-' : ' ';
  //o[0] = dec < 0 ? '-' : ' ';
  o[4] = '.';
  o[7] = 0;
  dec = abs(dec);
  for(n = 6; n > 4; n--) { o[n] = '0' + (dec % 10); dec /= 10; }
  o[3] = '0' + (dec % 10); dec /= 10;
  for(n = 2; n > 0; n--) { if(dec == 0) break; o[n] = '0' + (dec % 10); dec /= 10; }
  o[n] = sign;
  while(n--) o[n] = ' ';
}

// Fast integer square root
uint16_t isqrt(uint32_t a) {
  uint32_t rem = 0;
  uint32_t root = 0;
  for(unsigned i = 0; i < 16; i++) {
    root <<= 1;
    rem <<= 2;
    rem += a >> 30;
    a <<= 2;
    if(root < rem) {
        root++;
        rem -= root;
        root++;
    }
  }
  return root >> 1;
}
