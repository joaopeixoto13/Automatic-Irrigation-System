#include <REG51F380.H>
#include "i2c_bitbang.h"
#include "clock_i2c.h"

bit clk_wr_bytes(unsigned char addr ,unsigned char h,unsigned char m, unsigned char s)
{
	volatile i2c_transfer_t x;
	volatile unsigned char aux[4];
	aux[0] = addr;
	aux[1] = s;
	aux[2] = m;
	aux[3] = h;
	
	x.byte0 = CLK_WR;
	x.len = 4;
	x.byte_count = 0;
	x.payload = aux;

	
	if (!i2c_fifo_push(&x))
		return 0;
  
	while (TR0);
  
	return 1;
}

unsigned char clk_rd_byte(unsigned char addr)
{
	i2c_transfer_t xdata x;
	char dummy_write[1];
	char read[1];
    
	dummy_write[0] = addr;
	read[0] = 0;
    
	x.byte0 = CLK_WR;
	x.len = 1;
	x.byte_count = 0;
	x.payload = dummy_write;
    
	if (!i2c_fifo_push(&x))
		return 0;                                                         
										  
	x.byte0 = CLK_RD;
	x.len = 1;
	x.byte_count = 0;
	x.payload = read;

	if (!i2c_fifo_push(&x))
		return 0;
    
	while (TR0);
 
	return (10 *((x.payload[0] & 0xF0) >> 4)) + (x.payload[0] & 0x0F);
}