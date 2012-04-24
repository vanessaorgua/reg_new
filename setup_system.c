#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <stdio.h>

#include "delay.h"

#include "lcd.h"
#include "key.h"
#include "eep.h"

void setup_address();
void setup_speed();



prog_char menu5[]={0x48, 0x61, 0x63, 0xbf, 0x2e, 0x20, 0x63, 0xb8, 0x63, 0xbf, 0x65, 0xbc, 0xb8, 0x0}; // [0] "Наст. системи"
#define MAX_MENU 1

prog_char menu6[2][16]={
{0x41, 0xe3, 0x70, 0x65, 0x63, 0x61, 0x0}, // [0] "Адреса"
{0xac, 0xb3, 0xb8, 0xe3, 0xba, 0x69, 0x63, 0xbf, 0xc4, 0x0} // [1] "Швидкість"
};



void setup_system()
{
  register char i=0;
  while(1)
  {
	put_lcd_P(menu5,0);

	put_lcd_P(menu6[i],1);

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
						setup_address();
						break;
					case 1:
						setup_speed();
						break;

				}
		}
	
  
  }
  

}


void setup_address()
{

}

void setup_speed()
{


}



