#include "i2c_bitbang.h"
#include <REG51F380.H>

volatile states_tt data i2c_state, i2c_next_state;														// State Initialize
volatile i2c_transfer_t xdata* xdata i2c_transfer _at_ 0x100;												// Actual Transfer 

volatile i2c_buffer_t xdata i2c_push_buffer;														// I2C Push Buffer

volatile char count_bits = 8;																// Count bits variable
volatile unsigned char byte0;																// Save byte0 to shift 

sbit SCL = P2^4;																	// SCL Pin

sbit SDA = P2^5;																	// SDA Pin

void I2C_init(void)
{
	i2c_push_buffer.start = 0;															// Initialize I2C Push Buffer
	i2c_push_buffer.end = 0;
	i2c_push_buffer.len = 0;
	count_bits = 8;																	// Initialize 'count_bits'
}

char init_i2c_transfer(void)																// Function that initialize one Transfer				
{
	i2c_next_state = Start_L;
	SCL = 1;
	SDA = 1;
	TR0 = 1;
	ET0 = 1;
	EA = 1;
	return 0;
}

bit i2c_fifo_push(volatile i2c_transfer_t* x)
{
	if (i2c_push_buffer.len >= I2C_LEN)														// If buffer is full
		return 0;
	
	i2c_push_buffer.buffer[i2c_push_buffer.end++ & (I2C_LEN-1)] = *x;										// Add to buffer
	i2c_push_buffer.len++;																// Increment Buffer Length
	
	if (i2c_push_buffer.len == 1)															// If is empty
		init_i2c_transfer();															// Start a new Transfer
	
	return 1;
}

void timer0_isr(void) interrupt 1 using 2
{	
	SCL ^= 1;
	i2c_state = i2c_next_state;
	
	switch(i2c_state)
	{
		// **************** IDLE STATE *********************
		case Idle:
			TR0 = 0;															// Stop BitBang Timer
			SDA = 1;															// Ensure that SDA & SCL are in High Level
			SCL = 1;	
			break;
		
		
		// ************* START CONDITION *******************
		case Start_L:
			i2c_transfer = &i2c_push_buffer.buffer[i2c_push_buffer.start & I2C_LEN-1];							// Update actual transfer
			byte0 = i2c_transfer->byte0;
			SDA = 1;															// Ensure the SDA are in High Level to make the transition
			i2c_next_state = Start_H;
			break;
		
		case Start_H:
			SDA = 0;															// Make the Descendent Transition
			i2c_next_state = Byte0_L;
			break;
		
		
		// ************** SLAVE ADDRESS ********************
		case Byte0_L:
			SDA = byte0 & (1<<7);												// Grab the MSB bit		
			count_bits--;													// Decrement 'count_bits'	
			i2c_next_state = Byte0_H;
			break;
		
		case Byte0_H:
			if (!count_bits) {												// Byte complete 
				count_bits = 8;												// Reload 'count_bits'
				byte0 = i2c_transfer->byte0;										// Reload 'byte0' 
				i2c_next_state = RecvACK0_L;		
			}
			else {
				byte0 <<= 1;												// Next MSB
				i2c_next_state = Byte0_L;
			}
			break;
			
			
		// ************** SLAVE ADDRESS ACK ****************
		case RecvACK0_L:
			SDA = 1;													// Ensure SDA are High Level to slave pull-down 
			i2c_next_state = RecvACK0_H;
			break;
		
		case RecvACK0_H:
			if (!SDA) 													// If slave pull-down the line -> ACK
			{																	
				if ((i2c_transfer->byte0) & 0x01)									// Test Operation -> 1 - Read | 0 - Write
					i2c_next_state = RecvByte_L;									// Read Operation	
				else
					i2c_next_state = WriteByte_L;									// Write Operation
			}
			else														// Something gonna wrong
				i2c_next_state = Stop_L;
			break;
			
			
		// ************** READ OPERATION ********************
		case RecvByte_L:	
			i2c_next_state = RecvByte_H;
			count_bits--;													// Decrement 'count_bits'
			break;
		
		case RecvByte_H:
			i2c_transfer->payload[i2c_transfer->byte_count] <<= 1;								// Shift to fill the bits of 'n' byte in payload buffer
			i2c_transfer->payload[i2c_transfer->byte_count] |= SDA;								// Fill the position bit with SDA value
			if (!count_bits)
			{
				count_bits = 8;												// reload 'count_bits'								
				if (++i2c_transfer->byte_count == i2c_transfer->len)							// If all bytes are fill
					i2c_next_state = SendNACK_L;									// SendNack to finish 
				else												
					i2c_next_state = SendACK_L;										// SendACK to continue to fill the others bytes
			}
			else 														// If complete 8 bits transition
				i2c_next_state = RecvByte_L;
			break;
			
		
		// ************** WRITE OPERATION *******************
		case WriteByte_L:
			SDA = i2c_transfer->payload[i2c_transfer->byte_count] & (1<<7);							// Grab the MSB bit
			count_bits--;													// Decrement 'count_bits'	
			i2c_next_state = WriteByte_H;
			break;
		
		case WriteByte_H:
			if (count_bits)													// Byte not complete 
			{	
				i2c_transfer->payload[i2c_transfer->byte_count] <<= 1;							// Shift for next iteration 	!!!!!!!!!!!
				i2c_next_state = WriteByte_L;
			}
			else
			{
				count_bits = 8;												// Reload 'count_bits'
				i2c_next_state = RecvACK_L;
			}
			break;
			
			
		// **************** ACK OPERATIONS ******************
		case SendACK_L:
			SDA = 0;													// Pull the line to zero
			i2c_next_state = SendACK_H;
			break;
		
		case SendACK_H:
			i2c_next_state = RecvByte_L;
			break;
		
		case RecvACK_L:
			SDA = 1;													// Ensure SDA to slave pull-down 
			i2c_next_state = RecvACK_H;
			break;
		
		case RecvACK_H:
			if (!SDA)													// If Slave pull-down the line to zero
			{
				if (++i2c_transfer->byte_count == i2c_transfer->len)							// If all bytes were written
				{
					i2c_push_buffer.len--;
					i2c_push_buffer.start++;
					if (i2c_push_buffer.len) 									// If has more transfers to do
						i2c_next_state = Start_L;	
					else
						i2c_next_state = Stop_L;								// Generate Stop Condition
				}
				else
					i2c_next_state = WriteByte_L;									// Continue
			}										
			else
				i2c_next_state = Stop_L;										// Generate Stop Condition
			break;
			
			
		// **************** NACK OPERATIONS *****************
		case SendNACK_L:
			SDA = 1;
			i2c_next_state = SendNACK_H;
			break;
		
		case SendNACK_H:
			i2c_push_buffer.start++;											// Save Transfer on i2c_pop_buffer
			if (--i2c_push_buffer.len) 																										// If exist more Transfers to do - Write | Read
				i2c_next_state = Start_L;
			else																															// If complete all Transfers
				i2c_next_state = Stop_L;
			break;
			
			
		// ***************** STOP CONDITION *****************
		case Stop_L:
			SDA = 0;
			i2c_next_state = Stop_H;
			break;
		
		case Stop_H:
			SDA = 1;
			i2c_next_state = Idle;
			break;
	}
	TF0 = 0;															// TF0 flag clear
	
}