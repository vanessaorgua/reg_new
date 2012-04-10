#ifndef __EEP_H__
#define __EEP_H__

extern char EEMEM addr; //=1;

extern int EEMEM sca_lo[8]; //={0,0,0,0,0,0,0,0};
extern int EEMEM sca_hi[8]; //={4000,4000,4000,4000,4000,4000,4000,4000};

extern unsigned int EEMEM dac_hi[2];
extern unsigned char EEMEM dac_m[2]; // 0 -  "4-20" 1 - "0-20" 2..-"0-5"
extern unsigned char EEMEM md[2];

extern unsigned char EEMEM rev[2];

extern char EEMEM spd;

extern char EEMEM pn_en[2]; // пневматика відключена
#endif

