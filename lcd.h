#ifndef __LCD_H__
#define __LCD_H__

unsigned char init_LCD();

void put_lcd(char *str,unsigned char line);
void put_lcd_P(const prog_char *str,unsigned char line);

void byte2lcd(unsigned char byte,char _RS_);

#endif

