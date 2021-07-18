#ifndef __modes_module__
#define __modes_module__
#include "store_data.h"

// ********************************************** 
// **	               STATES	               **
// **********************************************

typedef enum states
{
	MODE_0 = 0,
	MODE_1,
	MODE_2,
	MODE_3,
	IDLE
} states_t;

// ********************************************** 
// **	       VARIABLES INITIALIZE            **
// **********************************************

extern states_t state;
extern states_t next_state;
extern char cmd;

extern volatile sensors_info_t xdata sensor;                           		//Estrutura de dados de temperatura e humidade
extern volatile timer_info_t xdata timer;                              		//Estrutura de dados sobre o tempo
extern volatile bit valvule_state;                                      	//Estado da valvula ... 1 - ON / 0 - OFF
extern volatile bit update;							//State Update


// ********************************************** 
// **	         FUNCTIONS PROTOTYPE	       **
// **********************************************

void menu(void);
void dummy(void);
void encode_FSM(void);

#endif