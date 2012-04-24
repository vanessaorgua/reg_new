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


extern unsigned char EEMEM adc_[8] ; // ={0x10,0x08,0x18,0x20,0x28,0x30,0x38,0x02}; // переадресація аналогових входів

extern unsigned int EEMEM ensca[2][2] ; //={ // тут буде зберігатися інженерна шкала
//{0,100},
// {0,100}
// };

extern unsigned char EEMEM enprz[2] ; //={0,0}; // кількість знаків після коми
extern unsigned char EEMEM enunit[2] ; //={0,0}; // вибір одиниць


#endif

