#include <stdio.h>
#include <REG51F380.H>
#include "modes.h"
#include "device_driver.h"
#define INVALID_HOUR 	 (timer.hour == -1 || timer.hour < 0 ||  timer.hour > 23)
#define INVALID_MINUTES  (timer.minutes == -1 || timer.minutes < 0 ||  timer.minutes > 59)
#define INVALID_DURATION (timer.duration == -1 || timer.duration < 0 ||  timer.duration > 90) 

// ********************************************** 
// **	         VARIABLES INITIALIZE	       **
// **********************************************

char code state_process[5][7] = {{7,6,6,6,4,5,6},
				{6,7,6,6,4,5,6},
				{6,6,8,6,4,5,6},
				{6,6,6,9,4,5,6},
				{0,1,2,3,6,6,6}};

states_t state;
states_t next_state;
char cmd;

volatile sensors_info_t xdata sensor;
volatile timer_info_t xdata timer;

volatile bit update;
sbit valvule_state = P1^4;



// ********************************************** 
// **	          MODES FUNCTIONS	       **
// **********************************************

void menu(void)
{
	valvule_state = 0;
	printf("Default -> mod0\r\nCustom  -> mod1\r\nTimer   -> mod2\r\nManual  -> mod3\r\n");
}

void dummy(void)
{
	valvule_state = 0;
	printf("Error: Invalid argument!\r\n");
}

void init_mod0(void)
{
	printf("Default mode active!\r\n");
	sensor.max_temp = DEFAULT_MAX_TEMP;                        										//Coloca valores default como parametros para a valvula
	sensor.min_humid = DEFAULT_MIN_HUMID;
	sensor.max_humid = DEFAULT_MAX_HUMID;
	next_state = MODE_0;
}

void init_mod1(void)
{                                          													//Utilizador define os parametros para a valvula
	printf("Insert your max temperature:\r\n");
	sensor.max_temp = get_numberdata();
	printf("Insert your min humidity:\r\n");
	sensor.min_humid = get_numberdata();
	printf("Insert your max humidity:\r\n");
	sensor.max_humid = get_numberdata();
	next_state = MODE_1;
	if( sensor.max_temp == -1 || sensor.min_humid == -1 || sensor.max_humid == -1 || sensor.min_humid >= sensor.max_humid)			//Caso algum dos parametros esteja errado será escolhido automaticamente o modo Default
	{
		printf("Incorrect parameters! -> Default mode will be chosen!\r\n");
		init_mod0();
	}else
		printf("Custom mode active!\r\n");
}

void init_mod2(void)
{
	printf("Insert your hour [0-23]:\r\n");
	timer.hour = get_numberdata();
	printf("Insert your minutes [0-59]:\r\n");
	timer.minutes = get_numberdata();
	printf("Insert irrigation duration(min) [0-90]:\r\n");
	timer.duration = get_numberdata();
	next_state = MODE_2;
	if(INVALID_HOUR || INVALID_MINUTES || INVALID_DURATION)
	{
		printf("Incorrect parameters!\r\n");
		next_state = IDLE;
		menu();
	}else
		printf("Timer mode active!\r\n");
}

void init_mod3(void)
{
	next_state = MODE_3;
	printf("Manual mode active!\r\n");
}

void swap(void)
{
	valvule_state = 0;															// segurança
	printf("Mode %d desactive!\r\n", (int)state);
	next_state = IDLE;
	menu();
}

void read(void)  
{	
	printf("Temperature: %dºC\r\nHumidity: %d%%\r\n",(int)sensor.actual_temp,(int)sensor.actual_humid);        				//Imprime os valores atuais da temperatura/humidade
	printf("valvule %d!\r\n",(int)valvule_state);
}

void info_modSensors(void)
{
	printf("Max Temperature: %dºC\r\nMin Humidity: %d%%\r\nMax Humidity: %d%%\r\n",(int)sensor.max_temp,(int)sensor.min_humid,(int)sensor.max_humid);
}

void info_modTimer(void)
{
	printf("The irrigation will start at %02dh%02dm for %d minutes\r\n",(int)timer.hour,(int)timer.minutes,(int)timer.duration);
}

void manual(void)
{
	valvule_state ^= 1;																							// ON / OFF da Válvula
	printf("valvule %d!\r\n",(int)valvule_state);
}

void encode_FSM(void) 
{
	switch(state_process[state][cmd])
	{
		case 0:
			init_mod0();
			break;
		case 1:
			init_mod1();
			break;
		case 2:
			init_mod2();
			break;
		case 3:
			init_mod3();
			break;
		case 4:
			read();
			break;
		case 5:
			swap();
			break;
		case 6:
			dummy();
			break;
		case 7:
			info_modSensors();
			break;
		case 8:
			info_modTimer();
			break;
		case 9:
			manual();
			break;
	}
}


