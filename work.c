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

extern int dac[2]; // Сигнал токового виходу.

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

#define UP_1 PD4
#define DW_1 PD5
#define UP_2 PD6
#define DW_2 PD7

SIGNAL(SIG_OUTPUT_COMPARE1A)
{				

		static unsigned char p=0;
		
		register unsigned char i,j;
		
		register union {
			unsigned int i;
			unsigned char c[2];
		} pack;

		long sum;

		int out[2]; // це для розрахунку виходу
		
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

		// визначення роботи клапана НО-НЗ
		if(eeprom_read_byte(rev))
		  out[0]=4000-dac[0];
		else
		  out[0]=dac[0];

		if(eeprom_read_byte(rev+1))
		  out[1]=4000-dac[1];
		else
		  out[1]=dac[1];

		  

		if(c_d)
		{
			ao.i[0]=(unsigned int)(((long)dac_k[0]*(long)out[0])>>BASE) + dac_o[0];
			ao.i[1]=(unsigned int)(((long)dac_k[1]*(long)out[1])>>BASE) + dac_o[1];
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
//управління пепі
// алгоритм із старої байпаски
// tmp  - завдання
// tmp1 - управління
//		UP_1 |= tmp1>tmp; // це включає виход
//		UP_1 &= tmp1>(tmp-zone[j]); // це виключає виход
//		DW_1 |= tmp1<=tmp; //!(tmp1>tmp)
//		DW_1 &= tmp1<(tmp+zone[j]); 
  //i=PORTD; // прочитати значеня порту

/* Алгоримт імені гранківського
  if(ai[1]<dac[0]-60) // включити набор
	PORTD |= _BV(UP_1);

  if(ai[1]>dac[0]) // виключити набор
	PORTD &=~_BV(UP_1);

  if(ai[1]>dac[0]+60) // включити набор
	PORTD |=_BV(DW_1);
  if(ai[1]<dac[0]) // виключити набор
	PORTD &=~_BV(DW_1);


  if(ai[3]<dac[1]-60) // включити набор
	PORTD |= _BV(UP_2);

  if(ai[3]>dac[1]) // виключити набор
	PORTD &=~_BV(UP_2);
                    
  if(ai[3]>dac[1]+60) // включити набор
	PORTD |=_BV(DW_2);
  if(ai[3]<dac[1]) // виключити набор
	PORTD &=~_BV(DW_2);
  */
  // спрощений алгоритм імені Нечипоренка
#define DELTA 40

  if(eeprom_read_byte(pn_en))
  {

	if(ai[1]>out[0]+DELTA)
	  sbi(PORTD,DW_1); // вклюсити стравлювання
    else
	  cbi(PORTD,DW_1);
	
    if(ai[1]<out[0]-DELTA)
  	  sbi(PORTD,UP_1); // включити набір
	else
		cbi(PORTD,UP_1); //
	}
	else
	{
//	  ai[1]=dac[0]; // заворот назад на комп
	  cbi(PORTD,DW_1);
		cbi(PORTD,UP_1); //
	
	}


  if(eeprom_read_byte(pn_en+1))
  {

  if(ai[3]>out[1]+DELTA)
	sbi(PORTD,DW_2); // вклюсити стравлювання
  else
	cbi(PORTD,DW_2);

  if(ai[3]<out[1]-DELTA)
	sbi(PORTD,UP_2); // включити набір
  else
	cbi(PORTD,UP_2); //
  }
  else
  {
//	ai[3]=dac[1]; // заворот назад на комп
	cbi(PORTD,DW_2);
	cbi(PORTD,UP_2); //
  }




	return ;
}

