#ifndef _I2C_BITBANG_
#define _I2C_BITBANG_
#define I2C_LEN 16

typedef enum STATES
{
	Idle = 0, 
	Start_L, Start_H, 
	Byte0_L, Byte0_H,
	RecvACK0_L, RecvACK0_H,
	RecvByte_L, RecvByte_H,
	WriteByte_L, WriteByte_H,
	SendACK_L, SendACK_H,
	RecvACK_L, RecvACK_H,
	SendNACK_L, SendNACK_H,
	Stop_L, Stop_H
} states_tt;


typedef struct I2C_TRANSFER
{
	unsigned char byte0;							// Slave ADDR
	unsigned char len;							// Number of Bytes to Read/Write
	unsigned char byte_count;						// For Bytes iteration
	unsigned char xdata * payload;						// Buffer -> Write - read from here to write | Read - save here
} i2c_transfer_t;


typedef struct I2C_BUFFER
{
	i2c_transfer_t buffer[I2C_LEN];						// I2C Transfers Buffer
	unsigned char start;							// Start Buffer
	unsigned char end;							// End Buffer
	unsigned char len;							// Buffer Length
} i2c_buffer_t;


void I2C_init(void);								// Init I2C
bit i2c_fifo_push(i2c_transfer_t* x);						// Push into I2C Buffer
bit i2c_fifo_pop(i2c_transfer_t* x);						// Pop for I2C Buffer


#endif