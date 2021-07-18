#include <REG51F380.H>
#include "config.h"

// ********************************************** 
// **	          ADC INITIALIZE	       **
// **********************************************

void ADC_Init()
{
	AMX0N     = 0x1F;              //SAR = 8MHz
	ADC0CF    = 0x28;              //AD0BUSY -> StartConversion
	ADC0CN    = 0x80;
	//AMX0P     = 0x01;            //P2.1
	//AMX0P     = 0x00;            //P2.0
	EIE1      = 0x08;
}

void Voltage_Reference_Init()
{
	REF0CN    = 0x08;
}


// ********************************************** 
// **	         UART INITIALIZE	       **
// **********************************************

void UART_Init()
{
	//SBRLL1    = 0x30;					
	//SBRLH1    = 0xFF;
	SBRLL1    = 0x3C;                                  	//Enable UART1 reception; Baudrate interno de 9600 bps
	SBRLH1    = 0xF6;
	SCON1     = 0x10;
	SBCON1    = 0x43;
}


// ********************************************** 
// **	        PORT_IO INITIALIZE	       **
// **********************************************

void Port_IO_Init()
{
	P2MDIN    = 0xFc;                                       // P2.0  -  Skipped,     Open-Drain, Analog
   	//P0SKIP    = 0x0F;                                     // P2.1  -  Skipped,     Open-Drain, Analog
	P0SKIP    = 0xFF;
	P1SKIP    = 0xFF;
	P2SKIP    = 0xFF;
	P3SKIP    = 0x03;
   	XBR1      = 0x40;                                       //P3.2 / P3.3
	XBR2      = 0x01;           				//Enable Crossbar; Pinos UART1
	EIE2 = 0x02;						// Enable da interrupção da porta série
}


// ********************************************** 
// **	       OSCILATOR INITIALIZE	       **
// **********************************************

void Oscillator_Init()
{
	FLSCL     = 0x90;					//48Mhz
   	CLKSEL    = 0x03;
}

// ********************************************** 
// **	        TIMER INITIALIZE	       **
// **********************************************


void Timer_Init()
{
	CKCON = 0X02;
	TMOD = 0x22;
	TH0 = -15;
	TL0 = -15;
	TH1 = -250;
	TL1 = -251;
	TR1 = 1;
	ET0 = 1;
	ET1 = 1;
}


// ********************************************** 
// **	       DEVICE INITIALIZE	       **
// **********************************************

void Init_Device(void)
{
   	PCA0MD = 0x00;
	Oscillator_Init();
	Timer_Init();
   	UART_Init();
	Port_IO_Init();
	ADC_Init();
	Voltage_Reference_Init();
}