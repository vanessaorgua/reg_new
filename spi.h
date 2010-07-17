#ifndef __SPI_H__
#define __SPI_H__

// ‘г«Ґа Єг«м­  дг­Єж?п ў?¤Їа ўЄЁ Ў ©в  Ї® SPI

static __inline__ char SPI_send(char x)
{
	char __ret;
	__asm__ __volatile__ (
	"out %2,%1 \t\n"
	"L_LOOP_%=: \t\n"
	"sbis %3,7 \t\n"
	"rjmp L_LOOP_%= \t\n"
	"in %0,%2 \t\n"
	: "=r" (__ret) 
	: "r" (x), "I" (_SFR_IO_ADDR(SPDR)) , "I" (_SFR_IO_ADDR(SPSR))
	);
	return __ret;
}

#endif


