#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <stdio.h>

#include "delay.h"

#include "lcd.h"
#include "key.h"
#include "eep.h"

extern char s[34];

extern prog_char menu2[4][17]; // тут є слово калібрування в 3 індексі
prog_char menu7[2][16]={
{0x42, 0x78, 0x69, 0xe3, 0x20, 0x25, 0x64, 0x0}, // [0] "Вхід %d"
{0x42, 0xb8, 0x78, 0x69, 0xb3, 0x20, 0x25, 0x64, 0x0} // [1] "Вихів %d"
};

#define MAX_MENU 9

void calibr_adc(char i);
void calibr_dac(unsigned char i);


void setup_calibr()
{
  register char i=0;
  while(1)
  {
	put_lcd_P(menu2[3],0);
	sprintf_P(s,menu7[i<8?0:1],i<8?i+1:i-7);
	put_lcd(s,1);
	
		switch(readkey())
		{
			case MIN:
				if(--i&0x80) i=MAX_MENU;
				break;
			case MAX:
				if(++i>MAX_MENU) i=0;
				break;
				
			case STOP:
				return;
			case SET:
				switch(i)
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
						calibr_adc(i);
						break;
					case 8:
					case 9:
						put_lcd(s,0);
						calibr_dac(i-8);
						break;
				}
		}
	
  }

}

prog_char msg1[]={0x4b, 0x61, 0xbb, 0x2e, 0x20, 0xb3, 0x78, 0x69, 0xe3, 0x20, 0x25, 0x64, 0x20, 0x25, 0x64, 0x0}; // [0] "Кал. вхід %d %d"

extern unsigned int f[8][16];

void calibr_adc(char i)
{
	register unsigned char j;
	register long sum;
	
	register unsigned int hi,lo;

	hi=eeprom_read_word(sca_hi+i);
	lo=eeprom_read_word(sca_lo+i);

	// перегрузити таблицю символів
	wait_key_release();
	
	while(1)
	{
		sum=0;
		for(j=0;j<16;++j)
			sum+=f[i][j];
		sum>>=4;

		sprintf_P(s,msg1,i+1,(int)sum);
		put_lcd(s,0);
		
		sprintf_P(s,PSTR(" \03=%4d \04=%4d"),lo,hi);
		put_lcd(s,1);
		
		switch(getkey())
		{
			case MIN:
				lo=sum;
				break;
			case MAX:
				hi=sum;
				break;
			case SET:
				eeprom_write_word(sca_hi+i,hi);
				eeprom_write_word(sca_lo+i,lo);
			case STOP:

				return;
				
		}
		wdt_reset();
		delay_ms(300);
	
	}
	
	return;

}

extern union {
	unsigned int i[2];
	unsigned char c[4];
} ao;

extern unsigned char c_d;

prog_char dacout[]={0x4D,0x61,0xBA,0x63,0x20,0xE1,0x41,0xA8,0x3D,0x25,0x34,0x64,0};
void calibr_dac(unsigned char i)
{
	c_d=0;

	ao.i[i]=eeprom_read_word(dac_hi+i);
	wait_key_release();
	
	while(1)
	{
		sprintf_P(s,dacout,ao.i[i]);
		put_lcd(s,1);

		switch(getkey())
		{
			case MIN:
				if(--ao.i[i]<3900) ao.i[i]=3900;
				break;
			case MAX:
				if(++ao.i[i]>4095) ao.i[i]=4095;
				break;
				
			case SET:
				eeprom_write_word(dac_hi+i,ao.i[i]);
			case STOP:
				c_d=1;
				return;
		}
		delay_ms(100);
		wdt_reset();
	}	

}
