#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <avr/eeprom.h>

#include "eep.h"

#define sbi(X,Y) X |=  _BV(Y)
#define cbi(X,Y) X &= ~_BV(Y)

#define TX_EN() 		sbi(PORTA,PA1); cbi(PORTC,PC3);
#define RX_EN()		    sbi(PORTC,PC3); cbi(PORTA,PA1);


unsigned char uart[35]; // uart buffer

unsigned char i=0;

#define CR '\r'

prog_char name[]="!00REGA\r";

prog_char sp[]={35	,23	,17	,11	,5	};

extern int ai[8];
extern unsigned int dac[2]; // Сигнал токового виходу.


void net_init()
{
// init uart
	sbi(DDRA,DDA1);
	RX_EN();

	UCSRB=(_BV(RXCIE) | _BV(TXCIE) | _BV(RXEN) ); // 
	UBRRL=pgm_read_byte(sp+eeprom_read_byte(&spd));

//	cbi(PORTA,PA1);
//	DDRD |= _BV(PD2);
}


static __inline__ unsigned char htoc(unsigned char hi,unsigned char lo) 
{
	return ((hi-(hi<'A'?'0':'7')) << 4) + (lo-(lo<'A'?'0':'7'));
}


SIGNAL(SIG_UART_RECV)
{
	register char s;
	static char pre=0;
	register int val;
	
	s=UDR;
	
	switch(s)
	{
			// Ї®з в®Є Ї ЄҐвг
			// § ЇЁвЁ ўҐ¤Ґз®Ј®
			case '$':
			case '%':
			case '#':
			case '~':
			case '@':
				pre=s;
				i=0;
				break;
			// ўi¤Ї®ўi¤i ўҐ¤®¬®Ј®
			case '!':
			case '?':
			case '>':
				pre=0;
				i=0;
				break;

			case CR: // Єi­Ґжм Ї ЄҐвг

				uart[i]=CR;
				uart[i+1]=0;
				if(pre==0) break;
//				eeprom_busy_wait();
//				s=eeprom_read_byte(&addr);
				s=1;
				if(htoc(uart[0],uart[1])!=s ) // addres check
					break;
				
				switch(pre)
				{
				
					case '$':
						switch(uart[2])
						{
							case 'M':
								strcpy_P(uart,name);
								break;

							case 'A':
								sprintf_P(uart,PSTR(">%04X%04X%04X%04X%04X%04X%04X%04X\r"),ai[0],dac[0],ai[2],dac[2],eeprom_read_byte(md),eeprom_read_byte(md+1),ai[1],ai[3]);
								break;
							
							case '2': // Read configuration
							  sprintf_P(uart,PSTR("!%0X300700\r"),s);
							  break;

							case '9':
							  sprintf_P(uart,PSTR("!%02X00\r"),s);
							  break;
							
							default:
								uart[0]='?';
								uart[1]=CR;
								uart[2]=0;
								break;
								
						}
						break;
						
					case '#':
						if(uart[2]=='\r') // це команда читання аналогових входів по типу І-7017
								sprintf_P(uart,PSTR(">%04X%04X%04X%04X%04X%04X%04X%04X\r"),ai[0],dac[0],ai[2],dac[1],eeprom_read_byte(md),eeprom_read_byte(md+1),ai[1],ai[3]);
						else // тут буде запис аналогових виходів по типу I-7024
						{
						  // тут треба прочитати дані із порту та записати у виходи.
							// розпакування даних

							s=uart[2]-'0'; //номер каналу
							if(eeprom_read_byte(md+s))
							{
								val=(int)(uart[3]-'0')*2000
									+(int)(uart[4]-'0')*200
									+(int)(uart[6]-'0')*20
									+(int)(uart[7]-'0')*2;
								dac[s]=val;
							}
							uart[0]='>';
							uart[1]=CR;
							uart[2]=0;
						}
							break;

					case '~':
					 	switch(uart[2])
						{
						case '0': // 	Read module status
							uart[2]=uart[1]; // зсунути адресу
							uart[1]=uart[0];

							uart[0]='!';
							uart[3]=uart[4]='0';
							uart[5]='\r';
							uart[6]=0;
							break;

						default:
							uart[0]='?';
							uart[1]='0';
							uart[2]='1';
							uart[3]=CR;
							uart[4]=0;
						}
                    break;

					  
					default:
							uart[0]='?';
							uart[1]='0';
							uart[2]='1';
							uart[3]=CR;
							uart[4]=0;
					
				} 
				UCSRB=(_BV(RXCIE) | _BV(TXCIE) | _BV(TXEN) ); // 
				TX_EN(); // enable driver
				UDR=uart[0];
				i=1;

				break;

			default:
				uart[i]=s;
				if(++i>34) i=0;
	}
	return;
}

SIGNAL(SIG_UART_TRANS)
{

	if(uart[i] && i<35 )
	{
		UDR=uart[i];
		++i;
	}
	else
	{
		RX_EN(); 
		UCSRB=(_BV(RXCIE) | _BV(TXCIE) | _BV(RXEN) ); // включити приймач.
	}

	return;
}
