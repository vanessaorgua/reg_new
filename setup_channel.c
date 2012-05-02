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

int input_value(int v,int decdig,char pos);


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


prog_char menu12[2][6]={
{0x4d, 0x69, 0xbd, 0x0}, // [0] "Мін"
{0x4d, 0x61, 0xba, 0x63, 0x0} // [1] "Макс"
};


prog_char menu13[]={0xac, 0xba, 0x61, 0xbb, 0x61, 0x20, 0xba, 0x61, 0xbd, 0x61, 0xbb, 0x20, 0x25, 0x64, 0x0}; // [0] "Шкала канал %d"
void setup_scale(char v)
{
  char p;
  int hi,lo;
  lo=eeprom_read_word(ensca[v]);
  hi=eeprom_read_word(ensca[v]+1);
  p=eeprom_read_byte(enprz+v);
  wait_key_release();

  sprintf_P(s+17,menu13,v+1);
  
  while(1)
  {
  
	sprintf_P(s,menu3,v+1);
	put_lcd(s,0);
	
	switch(p)
	{
		default: // це якщо із єпрома прочитано якісь фігню тоді
		  p=0;   // думати що там 0
		case 0:
		  sprintf_P(s,PSTR("%3d..%3d"),lo,hi);
		  break;
		case 1:
		  sprintf_P(s,PSTR("%2d.%1d..%2d.%1d"),lo/10,lo%10,hi/10,hi%10);
		  break;
		case 2:
		  sprintf_P(s,PSTR("%2d.%02d..%2d.%02d"),lo/100,lo%100,hi/100,hi%100);
		
	}
	put_lcd(s,1);
		switch(readkey())
		{
			case MIN:
				put_lcd(s+17,0);
				put_lcd_P(menu12[0],1);
				lo=input_value(lo,p,5);
				break;
			case MAX:
				put_lcd(s+17,0);
				 put_lcd_P(menu12[1],1);
				hi=input_value(hi,p,5);
				break;
				
			case SET:
				eeprom_write_word(ensca[v],lo);
				eeprom_write_word(ensca[v]+1,hi);
			case STOP:
				return;
			}
	
  }


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


prog_char pnen_msg[2][20]={
{0xA8,0x45,0xA8,0x49,0x20,0xB3,0xB8,0xBA,0xBB,0xC6,0xC0,0x65,0xBD,0x6F,0}, // "Pnevmo enable"
{0xA8,0x45,0xA8,0x49,0x20,0xB3,0xBA,0xBB,0xC6,0xC0,0x65,0xBD,0x6F,0} // "Pnevmo disble"
};

void setup_pepien(char v)
{

  register char i=eeprom_read_byte(pn_en+v);

  while(1)
  {
	put_lcd_P(pnen_msg+i,1);

	switch(readkey())
	{
	  case MIN:
		i=0;
		break;
	  case MAX:
		i=1;
		break;
	  case SET:
			eeprom_write_byte(pn_en+v,i);
	  case STOP:
		return;
	}

  }

}


prog_char menu14[]={0xa8, 0x45, 0xa8, 0x49, 0x20, 0xb7, 0x6f, 0xbd, 0x61, 0x3a, 0x20, 0x25, 0x31, 0x64, 0x2e, 0x25, 0x30, 0x32, 0x64, 0x0}; // [0] "ПЕПІ зона: %1d.%02d"


void setup_pepizone(char v)
{
  register unsigned char i;
  register int d;
  i=eeprom_read_byte(dpp+v); // прочитати
  
  while(1)
  {
    d=i;   // перешкалювати 200 сирих в 500 шкальованих
	d*=5;
    d/=2; 
    sprintf_P(s,menu14,d/100,d%100);
	put_lcd(s,1);
	
	switch(readkey())
	{
	  case MIN:
		i-=10;
		if(i>200) i=0;
		if(i==0) i=10;
		break;
	  case MAX:
		i+=10;
		if(i>200) i=200;
		break;
	  case SET:
			eeprom_write_byte(dpp+v,i);
	  case STOP:
		return;
	}

  
  }
  
}

