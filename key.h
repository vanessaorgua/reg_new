#ifndef __KEY_H__
#define __KEY_H__

#define SET  0xB0
#define STOP 0x70
#define MIN  0xD0
#define MAX  0xE0
#define getkey() (PINC&0xF0)

#define wait_key_release() 	while(getkey()!=0xF0) wdt_reset();
char readkey();


#endif

