#include <REG51F380.H>
#include "read_sensors.h"
#include "modes.h"
#include "types.h"


bit read_sensors_complete = 0;
break_integer16_t signal;
volatile break_integer32_t sensors_samp[2];
volatile unsigned int measure[2];

void Init_ADC (void)
{
	read_sensors_complete = 0;
	sensors_samp[0].window32 = 0x19191919;       //{25,25,25,25}
	sensors_samp[1].window32 = 0x19191919;       //{25,25,25,25}
}

void adc_cc(void) interrupt 10 
{
	signal.bits[0]=ADC0H;
	signal.bits[1]=ADC0L;
	AD0INT = 0;
	AMX0P ^= 1;
	if(AMX0P)
		measure[AMX0P] = ((((signal.value * 3.0)/ 1024.0) - 0.5) * 100.0);
	else
		measure[AMX0P] = 100 - ((signal.value / 1023.0) * 100.0) ;
	
	sensors_samp[AMX0P].window32 <<= 8;
	sensors_samp[AMX0P].window32 |= (char)measure[AMX0P];
	measure[AMX0P] = (unsigned char)sensors_samp[AMX0P].array[0] + (unsigned char)sensors_samp[AMX0P].array[1] + (unsigned char)sensors_samp[AMX0P].array[2] + (unsigned char)sensors_samp[AMX0P].array[3];
	measure[AMX0P] /= 4;
	sensor.actual_temp = (unsigned char)measure[1];
	sensor.actual_humid = (unsigned char)measure[0];
	read_sensors_complete = 1;
}


