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
						break;
					case 8:
					case 9:
						break;
				}
		}
	
  }

}
