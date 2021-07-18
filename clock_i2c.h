#ifndef _CLOCK_I2C_
#define _CLOCK_I2C_
#define CLK_WR 0xD0
#define CLK_RD 0xD1
#define SECONDS_ADDR 0x00
#define MINUTES_ADDR 0x01
#define HOURS_ADDR 0x02


bit clk_wr_bytes(unsigned char addr ,unsigned char h,unsigned char m, unsigned char s);
unsigned char clk_rd_byte(unsigned char addr);

#endif