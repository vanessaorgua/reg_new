#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <stdio.h>

#include "delay.h"

#include "lcd.h"
#include "key.h"
#include "eep.h"

#define MAX_MENU 3





extern char s[34];

char readkey()
{
	while(getkey()!=0xF0)
		wdt_reset();
	delay_ms(100);
	while(getkey()==0xF0)
		wdt_reset();
	return getkey();
}


void calc();
void setup_channel(char v);
void setup_system();
void setup_calibr();


prog_char menu1[]={0x48, 0x61, 0x63, 0xbf, 0x70, 0x6f, 0xb9, 0xba, 0x61, 0x0}; // [0] Настройка

prog_char menu2[4][17]={
{0x4b, 0x61, 0xbd, 0x61, 0xbb, 0x20, 0x31, 0x0}, // [0] Канал 1
{0x4b, 0x61, 0xbd, 0x61, 0xbb, 0x20, 0x32, 0x0}, // [1] Канал 2
{0x43, 0xb8, 0x63, 0xbf, 0x65, 0xbc, 0x61, 0x0}, // [2] Система
{0x4b, 0x61, 0xbb, 0x69, 0xb2, 0x70, 0x79, 0xb3, 0x61, 0xbd, 0xbd, 0xc7, 0x0} // [3] Калібрування

};

void setup()
{
	register unsigned char i=0;
		
	while(1)
	{
		put_lcd_P(menu1,0);
		put_lcd_P(menu2[i],1);
		
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
						setup_channel(i);
						break;
					case 2:
					  setup_system();
					  break;
					case 3:
					  setup_calibr();
					  break;

				}
		}
		
	}
	return;
}


