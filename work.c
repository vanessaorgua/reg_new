#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>


#include "eep.h"

#include "spi.h"

#define sbi(X,Y) X |=  _BV(Y)
#define cbi(X,Y) X &= ~_BV(Y)

#define CSA PB4
#define CSD PB2
#define LOAD PB3


#define CHANNEL_A 0x20
#define CHANNEL_B 0x30
#define CHANNEL_ALL 0x0


//prog_char adc[8]={0x08,0x10,0x18,0x20,0x28,0x30,0x38,0x02};
  prog_char adc[8]={0x10,0x08,0x18,0x20,0x28,0x30,0x38,0x02};

extern int ai[8]; // Шкальован? до 0...4000 аналогов? входи.

extern union {
	unsigned int i[2];
	unsigned char c[4];
} ao;

extern 
#if BASE  == 16
	long 
#else
	int
#endif
sca_k[8];

extern unsigned int dac[2]; // Сигнал токового виходу.

extern 
#if BASE == 16
	long 
#else
	int
#endif
dac_k[2];

extern int dac_o[2];

unsigned int f[8][16];
unsigned char c_d=1;

SIGNAL(SIG_OUTPUT_COMPARE1A)
{				

		static unsigned char p=0;
		
		register unsigned char i,j;
		
		register union {
			unsigned int i;
			unsigned char c[2];
		} pack;

		long sum;

				
		cbi(PORTB,CSA);
		SPI_send(0);
		SPI_send(0);
		sbi(PORTB,CSA);
		
		for(i=0;i<7;++i)
		{
			cbi(PORTB,CSA);
			pack.c[1]=SPI_send(pgm_read_byte(adc+i));
			pack.c[0]=SPI_send(0);
			sbi(PORTB,CSA);
			f[i][p]=pack.i;
			
			sum=0;

			for(j=0;j<16;++j)
			{
				sum+=f[i][j];
			}
			ai[i]=(((sum>>4)-(eeprom_read_word(sca_lo+i))) * sca_k[i]) >> BASE ;
			if(ai[i]>5000) ai[i]=5000;
//			else if(ai[i]<-1000) ai[i]=-1000;
			//ai[i]=sum>>4;
		}
		
		++p;
		p&=0x0f;	

		if(c_d)
		{
			ao.i[0]=(unsigned int)(((long)dac_k[0]*(long)dac[0])>>BASE) + dac_o[0];
			ao.i[1]=(unsigned int)(((long)dac_k[1]*(long)dac[1])>>BASE) + dac_o[1];
		}
		
		cbi(PORTB ,CSD);

		SPI_send(ao.c[1] | CHANNEL_B);
		SPI_send(ao.c[0]);

		sbi(PORTB,CSD);

		cbi(PORTB,LOAD);
		sbi(PORTB,LOAD);


		cbi(PORTB ,CSD);
		SPI_send(ao.c[3] |CHANNEL_A);
		SPI_send(ao.c[2]);
		sbi(PORTB,CSD);

		PORTB &=~_BV(LOAD);
		PORTB |= _BV(LOAD);

	return ;
}

