// Header for serial communications
//
// senseitg@gmail.com 2012-May-22

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// enumerate serial devices
// fp_enum is callback to receive each device
void ser_enum(void (*fp_enum)(char *name,char *device));

// open serial port
// device has system dependant form
// returns true if successful
bool ser_open(char* device);

// configure serial port
// fmt has form "baud,parity,databits,stopbit", ie: "9600,N,8,1"
// returns true if successful
bool ser_config(char* fmt);

// read from serial port
// returns bytes actually read
int32_t ser_read(void *p_read,uint16_t i_read);

// write to serial port
int32_t ser_write(void* p_write,uint16_t i_write);

// close serial port
bool ser_close();

#ifdef __cplusplus
}
#endif