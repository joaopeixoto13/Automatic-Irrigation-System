#include <REG51F380.H>
#include <stdio.h>
#include "config.h"
#include "device_driver.h"
#include "modes.h"
#include "read_sensors.h"
#include "clock_i2c.h"
#include "i2c_bitbang.h"
#include "display_i2c.h"
#define TIME_2S -8000
#define TIME_60S -30

volatile bit timerflag;

void timer1_isr() interrupt 3 using 2
{
	static volatile int vtimer = TIME_2S;
	static volatile int ctimer = TIME_60S;
	if(!++vtimer)
	{
		AD0BUSY = 1;
		vtimer = TIME_2S;
		
		if(!++ctimer)
		{
			timerflag = 1;
			ctimer = TIME_60S;
		}
	}
	TF0 = 0;
}

void main(void)
{       
	unsigned char duration;
	Init_Device();
	Init_UART();																					// Device Driver Initialize
	Init_ADC();
	I2C_init();
	lcd_init(16,2);
	next_state = IDLE;
	state = IDLE;																					
	EA = 1;																						// Enable das Interrupções
	timerflag = 0;
	menu();
	lcd_print("HUMD:       MOD",15);
	lcd_setCursor(0,1);
	lcd_print("TEMP:",5);
	while(1)
	{
		if(try_receive_message())																		// Se foi recebido uma mensagem
		{		
			cmd = get_keyword();																		// Processa a mensagem e descodifica-a																																	
			encode_FSM();																			// state machine																					
			state = next_state;
		}
		
		//read sensors
		if(read_sensors_complete)
		{
			read_sensors_complete = 0;
			if(state == MODE_0 || state == MODE_1)
			{
				if(sensor.actual_humid < (sensor.min_humid - HUMID_TOLERANCE))
					valvule_state = 1;
				if(sensor.actual_humid > (sensor.max_humid + HUMID_TOLERANCE))
					valvule_state = 0;
			}
			else if(state == MODE_2 && timerflag)
			{
				if(valvule_state && !--duration)
					valvule_state = 0;
				
				if(timer.hour == clk_rd_byte(HOURS_ADDR) && timer.minutes == clk_rd_byte(MINUTES_ADDR)){
					valvule_state = 1;
					duration = timer.duration;
				}
				timerflag = 0;
			}
			lcd_setCursor(6,0);
			lcd_send_number(sensor.actual_humid);
			lcd_send_char('%');
			lcd_setCursor(15,0);
			if(state == IDLE)
				lcd_send_char('#');
			else
				lcd_send_char(state + 48);
			lcd_setCursor(6,1);
			lcd_send_number(sensor.actual_temp);
			lcd_send_char(223);
			lcd_send_char('C');
			lcd_setCursor(15,1);
			lcd_send_char((char)valvule_state + 48);
		}		
	}
}