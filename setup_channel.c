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


prog_char menu3[]={0x48, 0x61, 0x63, 0xbf, 0x2e, 0x20, 0xba, 0x61, 0xbd, 0x61, 0xbb, 0x20, 0x25, 0x64, 0x0}; // [0] "Наст. канал %d"

#define MAX_MENU 6
prog_char menu4[7][16]={
{0x4f, 0xe3, 0xb8, 0xbd, 0xb8, 0xe5, 0x69, 0x0}, // [0] "Одиниці"
{0x50, 0x6f, 0xb7, 0x70, 0xc7, 0xe3, 0xb8, 0x0}, // [1] "Розряди"
{0xac, 0xba, 0x61, 0xbb, 0x61, 0x0}, // [2] "Шкала"
{0x4b, 0xbb, 0x61, 0xbe, 0x61, 0xbd, 0x0}, // [3] "Клапан"
{0x42, 0xb8, 0x78, 0x69, 0xe3, 0x0}, // [4] "Вихід"
{0xa8, 0x45, 0xa8, 0x49, 0x20, 0x79, 0xbe, 0x70, 0x61, 0xb3, 0xbb, 0x2e, 0x0}, // [5] "ПЕПІ управл."
{0xa8, 0x45, 0xa8, 0x49, 0x20, 0xb7, 0x6f, 0xbd, 0x61, 0x0} // [6] "ПЕПІ зона"
};


void setup_unit(char v);
void setup_prez(char v);
void setup_scale(char v);
void setup_valve(char v);
void setup_output(char v);
void setup_pepien(char v);
void setup_pepizone(char v);


void setup_channel(char v)
{
  register char i=0;
  while(1)
  {
	sprintf_P(s,menu3,v+1);
	put_lcd(s,0);

	put_lcd_P(menu4[i],1);

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
						setup_unit(v);
						break;
					case 1:
						setup_prez(v);
						break;
					case 2:
						setup_scale(v);
					  break;
					case 3:
						setup_valve(v);
					  break;
					case 4:
						setup_output(v);
					  break;
					case 5:
						setup_pepien(v);
					  break;
					case 6:
						setup_pepizone(v);
					  break;

				}
		}
	
  
  }
  

}

extern prog_char unit[5][2];
prog_char menu10[]={0x4f, 0xe3, 0xb8, 0xbd, 0xb8, 0xe5, 0x69, 0x3a, 0x20, 0x25, 0x63, 0x25, 0x63, 0x0}; // [0] "Одиниці: %c%c"
#define MAX_UNIT 4

void setup_unit(char v)
{
  register char i;
  i=eeprom_read_byte(enunit+v);
  
  wait_key_release();
  while(1)
  {
	sprintf_P(s,menu10,pgm_read_byte(unit[i]),pgm_read_byte(unit[i]+1));
	put_lcd(s,1);
		switch(readkey())
		{
			case MIN:
				if(--i&0x80) i=MAX_UNIT;
				break;
			case MAX:
				if(++i>MAX_UNIT) i=0;
				break;
				
			case SET:
				eeprom_write_byte(enunit+v,i);
			case STOP:
				return;
			}
  }
  

}

prog_char menu11[4][6]={"xxx","xx.x","xx.xx"};
#define MAX_PRZ 2

void setup_prez(char v)
{
  register char i;
  i=eeprom_read_byte(enprz+v);
  
  wait_key_release();
  while(1)
  {
		put_lcd_P(menu11[i],1);
		switch(readkey())
		{
			case MIN:
				if(--i&0x80) i=MAX_PRZ;
				break;
			case MAX:
				if(++i>MAX_PRZ) i=0;
				break;
				
			case SET:
				eeprom_write_byte(enprz+v,i);
			case STOP:
				return;
			}
  }

}


void setup_scale(char v)
{

}

prog_char modevt[2][18]={
{0x4B,0xBB,0x61,0xBE,0x61,0xBD,0x20,0x48,0x4F,0x28,0x42,0x4F,0x29,0}, // "NO","NC"
{0x4B,0xBB,0x61,0xBE,0x61,0xBD,0x20,0x48,0xA4,0x28,0x42,0xA4,0x29,0}
};

void setup_valve(char v)
{

  char i=eeprom_read_byte(rev+v);
  while(1)
  {
	put_lcd_P(modevt[i],1);
	switch(readkey())
	{
	  case MIN:
		i=0;
		break;
	  case MAX:
		i=1;
		break;
	  case SET:
		eeprom_write_byte(rev+v,i);
	  case STOP:
		return ;
	}
}

}

prog_char mode[3][20]={"4-20mA","0-20mA","0-5mA"};

void setup_output(char v)
{
  char i=eeprom_read_byte(dac_m+v);
  while(1)
  {
	put_lcd_P(mode[i],1);
	switch(readkey())
	{
	  case MIN:
		if(--i&0x80) i=2;
		break;
	  case MAX:
		if(++i>2) i=0;
		break;
	  case SET:
		eeprom_write_byte(dac_m+v,i);
	  case STOP:
		return ;
	}
  }
}


void setup_pepien(char v)
{

}


void setup_pepizone(char v)
{

}

