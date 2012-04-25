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



prog_char setaddr[]={0x41,0xE3,0x70,0x65,0x63,0x61,0x20,0x25,0x64,0};

void setup_address()
{
  char i=eeprom_read_byte(&addr);
  while(1)
  {
	sprintf_P(s,setaddr,i); // "Address %2d"
	put_lcd(s,1);
	switch(readkey())
	{
	  case MIN:
		if(--i<1) i=1;
		break;
	  case MAX:
		if(++i>100) i=100;
		break;
	  case SET:
		eeprom_write_byte(&addr,i);
	  case STOP:
		return;

	}
  }
  
}


prog_char speed[5][10]={"9600","19200","38400","57600","115200"};

void setup_speed()
{

  register char i=eeprom_read_byte(&spd);
  // put_lcd_P(menu[6],0);
  while(1)
  {
	put_lcd_P(speed[i],1);
	switch(readkey())
	{
	  case MIN:
		if(--i&0x80) i=4;
		break;
	  case MAX:
		if(++i>4) i=0;
		break;
	  case SET:
		  eeprom_write_byte(&spd,i);
		  put_lcd_P(PSTR("Restart!"),1);
		  while(1); // перезапуск програми
	  case STOP:
		return;
	}
	
  }


}



