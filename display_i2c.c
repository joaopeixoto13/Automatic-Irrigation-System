#include "display_i2c.h"
#include <REG51F380.H>
#include "i2c_bitbang.h"

volatile unsigned char _Addr;
volatile unsigned char _displayfunction;
volatile unsigned char _displaycontrol;
volatile unsigned char _displaymode;
volatile unsigned char _numlines;
volatile unsigned char _cols;
volatile unsigned char _rows;
volatile unsigned char _backlightval;


void delay(unsigned int y){
	unsigned int i;
	while(y-- > 0){
		i = 0;
		while(i++ < 48);
	}

}

void expanderWrite(unsigned char _data){ 
	i2c_transfer_t x;
	_data |= _backlightval;
	x.byte0 = DISPLAY_WR;
	x.len = 1;
	x.byte_count = 0;
	x.payload = &_data;
	i2c_fifo_push(&x);
	while(TR0);
} 

void pulseEnable(unsigned char _data){
	expanderWrite((_data | En ));	
	delay(1);		
	expanderWrite(_data);	
	delay(50);		
}

void write4bits(unsigned char value) {
	expanderWrite(value);
	pulseEnable(value);
}

void send(unsigned char value, unsigned char mode) {             
	unsigned char highnib=value&0xf0;
	unsigned char lownib=(value<<4)&0xf0;
	write4bits((highnib)|mode);
	write4bits((lownib)|mode); 
}

void lcd_command(unsigned char value) {
	send(value, 0);
}

void lcd_send_char(unsigned char value) {
	send(value, 1);
	delay(100);
}

void lcd_send_number(unsigned char value) {
	
	unsigned char x;
	
	x = value / 10;
	send(x + 48, 1);
	delay(100);
	x = value % 10;
	send(x + 48, 1);
	delay(100);
}

void lcd_print(unsigned char* value, unsigned char size){
	unsigned char i;
	char aux [20];
	for(i = 0;  i < size && i<16; i++)
		aux[i] = value[i];
	for(i = 0;  i < size && i<16; i++)
		lcd_send_char(aux[i]);
}
void lcd_clear(){
	lcd_command(LCD_CLEARDISPLAY);
	delay(2000);  
}

void lcd_home(){
	lcd_command(LCD_RETURNHOME);  
	delay(2000);  
}

void lcd_setCursor(unsigned char col, unsigned char row)
{
	unsigned char maskData;
	maskData = (col)&0x0F;
	if(row==0)
	{
		maskData |= (0x80);
		lcd_command(maskData);
	}
	else
	{
		maskData |= (0xc0);
		lcd_command(maskData);
	}
}

void lcd_init(unsigned char cols, unsigned char rows) {
	_Addr = DISPLAY_WR;
	_cols = cols;
	_rows = rows;
	_backlightval = LCD_BACKLIGHT;
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	if (rows > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = rows;

	delay(60000);

	write4bits(0x30);
	delay(7000);
	write4bits(0x30);
	delay(7000);
	write4bits(0x30);
	delay(7000);
	write4bits(0x20);
	delay(7000);
	
	lcd_command(0x28);
	delay(5000);
	lcd_command(0x08);
	delay(5000);
	lcd_command(0x01);
	delay(5000);
	lcd_command(0x06);
	delay(5000);
	lcd_command(0x0C);
	delay(20000);
}