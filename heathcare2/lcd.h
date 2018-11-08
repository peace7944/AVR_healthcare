#define ENABLE  (PORTC |= 0x04)
#define DISABLE (PORTC &= 0xFB)

void init_PORTC(void)
{
  DDRC = 0xff;
  PORTC = 0xff;
}
 
void instrction_out(unsigned char b)
{
	PORTC = b&0xF0;
	ENABLE;
	DISABLE;
	PORTC=((b<<4)&0xF0);
	ENABLE;
	DISABLE;
	_delay_ms(2);
}
void char_out(unsigned char b)
{
	PORTC=(b&0xF0)|0x01;
	ENABLE;
	DISABLE;
	PORTC=((b<<4)&0xF0)|0x01;
	ENABLE;
	DISABLE;
	_delay_ms(2);
}
void lcd_gotoxy(unsigned char x, unsigned char y)
{
	// 16 * 2 Character LCD  
	//  Busy();
	switch(y)
	{
	    case 0 : instrction_out(0x80+x); break;
	    case 1 : instrction_out(0xC0+x); break;
	}
}
void lcd_putsf(unsigned char x, unsigned char y, unsigned char *str)
{
	unsigned int i=0;
	lcd_gotoxy(x,y);
	do
	{
		char_out(str[i]);
	}
	while(str[++i]!='\0');
}
void lcd_init(void)
{
	init_PORTC();
	_delay_ms(200);
	_delay_us(600);
	instrction_out(0x28);
	_delay_ms(200);
	instrction_out(0x28);
	_delay_ms(200);
	instrction_out(0x0C);
	instrction_out(0x06);
	instrction_out(0x02);
	instrction_out(0x01);
	instrction_out(0x01);
}
void lcd_clear(void)
{
	instrction_out(0x01);
	_delay_ms(10); 
}
