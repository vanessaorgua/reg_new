#include <avr/io.h>
#include <avr/pgmspace.h>

#include "delay.h"

#define sbi(X,Y) X |=  _BV(Y)
#define cbi(X,Y) X &= ~_BV(Y)

#define BUS PORTA
#define RS PA2
//#define RW PA1
#define E  PA3

void impe(unsigned char b);
//void db_wr(unsigned char b);
#define CHAR_WAIT 40

void byte2lcd(unsigned char byte,char _RS_)
{
	if(_RS_)
		sbi(BUS,RS);
	else
		cbi(BUS,RS);

	asm volatile ("nop");
	asm volatile ("nop");

	
//	db_wr(byte);
	impe(byte&0xF0);
	impe(byte<<4);

	delay_us(CHAR_WAIT);
	
}


unsigned char init_LCD()
{	
	DDRA=0xFF;
	
	BUS=8;
		
	delay_ms(15);
	impe(3<<4);
	delay_ms(5);
		
	impe(3<<4);
	delay_us(120);

	impe(3<<4);
	delay_us(CHAR_WAIT);
	
	impe(2<<4);
	delay_us(CHAR_WAIT);

	byte2lcd(12,0);
//    db_wr(12); // display on
//	delay_us(CHAR_WAIT);
   
	byte2lcd(6,0);
//    db_wr(6);  // entry mode set  
//	delay_us(CHAR_WAIT);

	byte2lcd(1,0);
//    db_wr(1);  // display clear
//	delay_us(CHAR_WAIT);	
	
	return 0;
}


void put_lcd(char *str,unsigned char line)
{
    register unsigned char i=0;
	register char s;

	byte2lcd(128+64*line,0);      

    s=str[0];
    while(s)
     {
		byte2lcd(str[i],1);      
        
        ++i;
        s=str[i]; 
     }

    for(;i<16;i++)
	{
		byte2lcd(' ',1);      
        
	}

}



void put_lcd_P(const prog_char *str,unsigned char line)
{
    register unsigned char i=0;

	register char s;

	byte2lcd(128+64*line,0);      
      

    sbi(BUS,RS); //rs=1;
    s=pgm_read_byte(str);      
    while(s)
     {
   
		byte2lcd(s,1);      
        ++i; 
	    s=pgm_read_byte(str+i);      
        
     }

    for(;i<16;i++)
	{
		byte2lcd(' ',1);      
	}

//     cbi(BUS,RS); //=0;
}



void impe(unsigned char b)
{
//	register char m;

//	m=MCUCR;
	asm("cli");
	BUS = (BUS&0x0F)|b;
	asm("sei");
	
//	MCUCR=m;
	
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");

	sbi(BUS,E);
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	cbi(BUS,E);
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
}

/*
void db_wr(unsigned char b)
{
	impe(b&0xF0);
	impe(b<<4);
}
*/

