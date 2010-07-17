#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include <stdio.h>

#include "spi.h"

#include "lcd.h"

#include "delay.h"

#include "key.h"

#define sbi(X,Y) X |=  _BV(Y)
#define cbi(X,Y) X &= ~_BV(Y)


//---------------------------------------------------------------------------------------------------------------------------
// Дан? в EEPROM

//---------------------------------------------------------------------------------------------------------------------------
char EEMEM addr=1;

int EEMEM sca_lo[8]= {56,890,1055,38,0,0,0,0};//{0,0,0,0,0,0,0,0};
int EEMEM sca_hi[8]={4000,4000,4000,3338,4000,4000,4000,4000};

unsigned int EEMEM dac_hi[2]={4000,4000};
unsigned char EEMEM dac_m[2]={0,0}; // 0 -  "4-20" 1 - "0-20" 2..-"0-5"

unsigned char EEMEM md[2]={0,0};
int EEMEM val[2]={0,0};
 
//---------------------------------------------------------------------------------------------------------------------------
// Дан?в в ОЗУ
int ai[8]={0,0,0,0,0,0,0,0}; // Шкальован? до 0...4000 аналогов? входи.

unsigned int dac[2]={0,0}; // Сигнал токового виходу.

union {
	unsigned int i[2];
	unsigned char c[4];
} ao;

#if BASE == 16
	long 
#else
	int
#endif
sca_k[8];

// Коеф?ц?єнти шкалюваня дак.
#if BASE == 16
	long 
#else
	int
#endif

dac_k[2];
int dac_o[2];


char s[34]; // буфер LCD

//---------------------------------------------------------------------------------------------------------------------------
void net_init();

//---------------------------------------------------------------------------------------------------------------------------
void setcg(unsigned char a,const char *sym)
{
	register unsigned char i;
	
	byte2lcd(0x40 + (a*8),0);

	for(i=0;i<8;++i)
		byte2lcd(pgm_read_byte(sym+i),1);
	return;
}

	prog_char valve[]={0x0E,0x0E,0x04,0x15,0x1B,0x1F,0x1B,0x11};
	prog_char arrov[]={0,8,4,30,4,8,0,0};
    prog_char up[]={0,4	,14	,21	,4	,4	,4	,0	};
    prog_char dw[]={0	,4	,4	,4	,21	,14	,4,0	};
    prog_char tg[]={0x00,0x1C,0x09,0x0A,0x04,0x0B,0x12,0x02};
    prog_char mg[]={0x18,0x08,0x11,0x0A,0x14,0x0b,0x12,0x02};
	prog_char grad[]={2	,5	,2	,0	,0	,0	,0	,0	};

void lcd_init()
{
// знакогенератор розм_щується в EEPROM

	init_LCD();
	delay_ms(10);
	
	setcg(2,arrov);
	setcg(1,valve);
	setcg(3,up);
	setcg(4,dw);
	setcg(5,tg);
	setcg(6,mg);
	setcg(7,grad);

}



void setup();

void calc()
{
	unsigned char j;

	for(j=0;j<8;++j)
	{
		sca_k[j]= (4000l<<BASE) / (long)(eeprom_read_word(sca_hi+j)-eeprom_read_word(sca_lo+j));
	}	

	for(j=0;j<2;++j)
	{
		switch(eeprom_read_byte(dac_m+j))
		{
			case 0: // 4-20
				dac_o[j]=eeprom_read_word(dac_hi+j)/5;
				
				dac_k[j]=(((long)(eeprom_read_word(dac_hi+j)-dac_o[j]))<<BASE)/4000l;
				break;
			case 1: // 0-20
				dac_k[j]=(((long)(eeprom_read_word(dac_hi+j)))<<BASE)/4000l;
				dac_o[j]=0;
				break;
			default: // 0-5
				dac_k[j]=(((long)(eeprom_read_word(dac_hi+j)/5))<<BASE)/4000l;
				dac_o[j]=0;
				
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------
int main()
{
	unsigned char ch=0;
	
	wdt_enable(WDTO_500MS);
	
	PORTB=0x1C;
 	DDRB= 0xBC;

		
 	
	DDRD = 0xF0;

	DDRC = 0x0F;
	PORTC= 0xF8;

	
	sbi(SPSR,SPI2X);
	SPCR= _BV(SPE) | _BV(MSTR)  | _BV(CPOL) | _BV(CPHA) | _BV(SPR0); //

	calc();
	net_init();
	
	lcd_init();

	
	OCR1A=675;
	TCCR1A=0;
	TCCR1B= _BV(WGM12) | _BV(CS12) | _BV(CS10); 
	
	sbi(TIMSK,OCIE1A);
	

	dac[0]=eeprom_read_word(val);
	dac[1]=eeprom_read_word(val+1);

	wdt_reset();	

	sei();

	while(1)
	{

		 
		
		sprintf(s," %3d%% %3d%%\01%3d%%%c",ai[0]/40,ai[1]/40,dac[0]/40,eeprom_read_byte(md)?'A':'P');
		put_lcd(s,0);

		sprintf(s," %3d%% %3d%%\01%3d%%%c",ai[2]/40,ai[3]/40,dac[1]/40,eeprom_read_byte(md+1)?'A':'P');
		put_lcd(s,1);

		byte2lcd(128+64*ch,0);
		byte2lcd(2,1);
		

		switch(getkey())
		{
			case SET & STOP:
				setup();
				calc();
				break;
			case STOP:
				ch ^=1;
				break;
			case SET:
				if(eeprom_read_byte(md+ch))
				{
					eeprom_write_byte(md+ch,0);
					dac[ch] = ((dac[ch]/200))*200;
				}
				else
					eeprom_write_byte(md+ch,1);
				break;
			case MIN:
				if(!eeprom_read_byte(md+ch))
				{
					dac[ch]-=200;
					if(dac[ch]&0x8000) dac[ch]=0;
					eeprom_write_word(val+ch,dac[ch]);
				}
				break;
			case MAX:
				if(!eeprom_read_byte(md+ch))
				{
					dac[ch]+=200;
					if(dac[ch]>4000) dac[ch]=4000;
					eeprom_write_word(val+ch,dac[ch]);
				}
			
		}

		delay_ms(300);
		wdt_reset();
	}
	
	return 0;
}

