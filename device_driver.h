#ifndef __device_driver_module__
#define __device_driver_module__
#define LEN 128
#define TI1 1									 
#define RI1 0
#define ES1 1

// ********************************************** 
// **	  STRUCT BUFFER FOR DEVICE DRIVER      **
// **********************************************

typedef struct Buffer
{				  				//Tipo de variavel Buffer para controlar ambos os buffer de recessao e trasmissao
	 char buffer [LEN];					//Buffer
	 unsigned char start,end;				//Start index do buffer e end index do buffer
	 unsigned char buffer_length; 				//Numero de posicoes do buffer ocupadas
} Buffer;


// ********************************************** 
// **	       PROTOTYPE OF FUNCTIONS	       **
// **********************************************

void Init_UART(void);						//Funcao de inicializacao do device driver
char _getkey (void);						//Funcao para leitura do buffer de recessao
char get_keyword(void);       					//Devolve a keyword recebida
bit try_receive_message(void);					//Devolve a ocorrencia de um \r
char get_numberdata(void);                  			//Devolve o numero digitado

#endif