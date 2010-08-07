#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/crc16.h>

#include <avr/pgmspace.h>

#include <avr/eeprom.h>

#include "eep.h"


#define sbi(X,Y) X |=  _BV(Y)
#define cbi(X,Y) X &= ~_BV(Y)


#define READREG 03
#define PRESSINGLEREG 06
#define PRESSMULTIREG 16


unsigned char uart[128]; // buffer
unsigned char volatile i=0,len=0;
unsigned int  volatile crc=0xFFFF;


static __inline__ int swap_char(int value)
{
	int v;

	v=value;
 	
 	__asm__ __volatile__ (
	"mov __tmp_reg__,%A0 \n\t"
	"mov %A0,%B0 \n\t"
	"mov %B0,__tmp_reg__ \n\t"
	:"=r"(v)
	:"0"(v)
	);
	return v;
}


#define pack(x,y) ((x<<8)+y)

//extern unsigned int cnt;
extern char EEMEM addr;

#define BAUD 35

void net_init()
{
	UBRRL = BAUD; // 115200 bps
	UCSRC = 0x86;
	UCSRB = _BV(RXCIE) | _BV(TXCIE) | _BV(RXEN) | _BV(TXEN);
	
	TCCR0 = 0;  // stop timer

	OCR0  = 2.5*BAUD+2.5;  // six byte 
	
	TIMSK |= _BV(OCIE0); // enable timer interrupt

	sbi(DDRA,DDA1);
	cbi(PORTA,PA1);

}

SIGNAL(SIG_UART_RECV)
{
	TCCR0= 0;// _BV(CS02) | _BV(CS00); // run timer0		
	TCNT0 = 0;
	TCCR0= _BV(CS02);// | _BV(CS00); // run timer0		

	uart[i]=UDR;
	crc=_crc16_update(crc,uart[i]);
	++i;


}

extern int ai[8];
extern unsigned int dac[2]; // Сигнал токового виходу.

SIGNAL(SIG_UART_TRANS)
//SIGNAL(SIG_UART1_DATA)
{
	++i;
	if(i<len)
	{
		UDR=uart[i];
		crc=_crc16_update(crc,uart[i]);
	}	
	else if(i==len)
	{
		UDR=crc&0x00FF;
	}
	else if(i==(len+1))
	{
		UDR=crc>>8;
	}
	else 
	{
		i=0;
		crc=0xFFFF;
		sbi(PORTC,PC3);
		cbi(PORTA,PA1);
	}
}


SIGNAL(SIG_OUTPUT_COMPARE0) //
{
	register unsigned char f,j,b,n;
	register unsigned int  adr,t;
	register unsigned int *ptr;
	register unsigned char *p;
	
	
	TCCR0=0; // stop timer
	i=0;
	adr=crc; // save crc

	crc=0xFFFF;			
	
	if(uart[0]!=eeprom_read_byte(&addr))
	{
		return;
	}
	if(adr) // analize saved crc
	{
		return;
	}

//	cbi(PORTD,PD4);
//	cbi(PORTD,PD5);
//	cbi(PORTD,PD6);
//	cbi(PORTD,PD7);

	f=uart[1];
	ptr=(unsigned int*)(uart+2);
	
	adr=pack(uart[2],uart[3]);
	
	if(f==READREG)
	{
		
		b=uart[3];		
//		cnt=
		n=uart[5];
		if(n>32)
		{
			return ; 
		}
		uart[2]=n*2;
		ptr=(unsigned int*)(uart+3); // Set pointer to data

		for(j=0;j<n;++j)
		{		
			//ptr[j]=j;
			p = (unsigned char*)(uart+(3+j*2));

			switch( (adr+j))
			{

				case 0:
				case 1:
				case 2:
				case 3:
/*
				case 4:
				case 5:
				case 6:
				case 7: */
					p[0]=(unsigned char)((ai[adr+j])>>8);
					p[1]=(unsigned char)(ai[adr+j]&0x00FF);
					break;
				case 4: //8:
				case 5: //9:
					p[0]=(unsigned char)((dac[adr+j-4])>>8);
					p[1]=(unsigned char)(dac[adr+j-4]&0x00FF);
					break;

				case 6: //10:
				case 7: //11:
					p[1]=eeprom_read_byte(md+adr+j-6);
					p[0]=0;
					break;

				case 8:
				case 9:
				case 10:
				case 11:
					p[0]=(unsigned char)((ai[adr+j-4])>>8);
					p[1]=(unsigned char)(ai[adr+j-4]&0x00FF);
					break;
				
				default:
					p[0]=0xFF;
					p[1]=0xFF;
			}
		}
		
		len=3+n*2;
	}
	else if(f==PRESSMULTIREG)
	{
//		adr=ptr[0];
		n=uart[5];

		ptr=(int*)(uart+7);
		
		for(j=0;j<n;++j)
		{
			
			t= swap_char(ptr[j]);

			switch((adr+j))
			{					
				case 4:
				case 5:
					if(eeprom_read_byte(md+adr+j-4))
						dac[adr+j-4]=t;
					break;
//				case 0xFFFF:
//					asm("jmp 0x7C00");
//					break;			
			}
		}
		
		len=6; // normalli responce is the head of package
	}
  	else
	{
		return;
	}

	sbi(PORTA,PA1);
	
	cbi(PORTC,PC3);
	
	crc=_crc16_update(crc,uart[0]);
	UDR=uart[0];
}


