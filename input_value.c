#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "lcd.h"
#include "key.h"

extern char s[34];

int input_value(int v,int decdig,char pos)
{

  char sim[5]; // буфер для введених символів, число розкладається із кінця
  register char i,sp=0;
  int old_val;

  wait_key_release();

  old_val=v; // зберегти про всяк випадок

  for(i=0;i<5;++i) // розкласти число на розраяди та скоригувати до символів '0'-'9'
  {
    sim[i]=v%10+'0'; // наймолодша цифра
  	v/=10; // це типу десятковий зсув на один десятковий розряд
  }

  i=128+64+pos;
  byte2lcd(i,0); // виставили курсор в портрібну позицію.

  for(i=4;i>0;--i)
  {
	byte2lcd(sim[i],1);
	//byte2lcd(i+'0',1);
	if(i==decdig)
	  byte2lcd('.',1);
  }
	byte2lcd(sim[0],1);

  i=128+64+pos;
  byte2lcd(i,0); // виставили курсор в портрібну позицію.

  byte2lcd(0x0F,0) ; // включити курсор


  while(1)
  {
		switch(readkey())
		{
			case MIN:
				if(--sim[sp]<'0') sim[sp]='9';
				break;
			case MAX:
				if(++sim[sp]>'9') sim[sp]='0';
				break;
				
			case STOP:
				byte2lcd(0x0C,0); // виключити курсор
				return old_val;
				
			case SET:
				if(++sp>4) // перейти до наступної цифри або якщо
				{ // обробили всі  цифри - скласти число та завершити роботу.
					v=0;
  	              for(i=0;i<5;++i)
    	            {
        	      	  v*=10;
            	  	  v+=sim[i]-'0';
              	  }
					byte2lcd(0x0C,0); // виключити курсор
					return v; // v попереднб
				}
				else // тут треба буде пересунути курсор
				{
				
				}
			}
	
  }



}

