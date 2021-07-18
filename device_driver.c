#include <REG51F380.H>
#include <stdio.h>
#include "device_driver.h"
#include "config.h"

Buffer xdata RX;												// Buffer de receção																																					
Buffer xdata TX;												// Buffer de transmissão																																
bit TXactive = 0;												// _bit responsável por saber se está a transmitir
bit RXactive = 0;												// _bit responsável por saber se está a receber


// ********************************************** 
// **	      UNION TO PROCESS KEYWORDS	       **
// **********************************************

typedef union byte_interger32
{													
	char byte[4];
	long int integer32;
} byte_integer32_t;



// ********************************************** 
// **	         KEYWORDS ARRAY	               **
// **********************************************
//                            "mod0"      "mod1"      "mod2"     "mod3"      "read"      "swap"       "dummy"
code long int keyword[7] = {0x6D6F6430, 0x6D6F6431, 0x6D6F6432, 0x6D6F6433, 0x72656164, 0x73776170, 0xFFFFFFFF};
														


// ********************************************** 
// **	         Inicialize() Function	       **
// **********************************************

void Init_UART (void) 
{														//Inicia ambos os buffers vazios 
  	RX.start = 0;                                  
  	RX.end = 0;  
	RX.buffer_length = 0;											
  	TX.start = 0;                                  
  	TX.end = 0; 
	TX.buffer_length = 0;
	TXactive = 0;												//Nao está a ocorrer trasmissão
	RXactive = 0;												//Ocorrencia de /r
}

// ********************************************** 
// **	         UART1_ISR() function	       **
// **********************************************

void UART_ISR (void) interrupt 16 
{
	char c;
	
	//Received value interrupt_
	
  	if(SCON1 & (1<<RI1))
	{   													
		c = SBUF1;											//Limpa a flag de recessao					          
		if (RX.buffer_length < LEN) 
		{
			RX.buffer[RX.end++ & (LEN-1)] = c; 							//Coloca o caracter no buffer
			RX.buffer_length++;									//Aumenta o tamanho ocupado do buffer
		}
		SCON1 &= ~(1<<RI1);	
  	}
		
	//Transmitted value interrupt_
		
	if ((SCON1 & (1<<TI1))) 
	{                     											             
		if (TX.buffer_length)
		{												//Se ainda tiver caracteres no buffer
			SBUF1 = TX.buffer[TX.start++ & (LEN-1)];  						//Envia o caracter do buffer pela porta serie
			TX.buffer_length--;									//Diminui o tamanho ocupado do buffer
		}
		else                                     							//Se todos os caracteres foram enviados o _bit da trasmissao fica 0
    		TXactive = 0;                         					
		SCON1 &= ~(1<<TI1);  
	}
}


// ********************************************** 
// **	         putchar() function	       **
// **********************************************

char putchar (char c) 
{
	while (TX.buffer_length == LEN);           								//Espera ate ter espaço no buffer                                            
	if (!TXactive) 
	{                         	 									//Transmite diretamente o primeiro caracter para ablitar automaticamente o envio dos outros todos do buffer
		TXactive = 1;                        
		SBUF1 = c;                             
	}
	else 
	{
		EIE2 &= ~(1<<ES1);                        							//Desativa a interrupcão serial            
		TX.buffer[TX.end++ & (LEN-1)] = c;      							//Coloca o caracter no buffer de transmissao   
		TX.buffer_length++;                 								//Aumenta o tamanho do buffer              
		EIE2 |= (1<<ES1);                        							//Ativa as interrupcão serial       
	}                                 
	return (c);
}



// ********************************************** 
// **	         _getkey() function	       **
// **********************************************

char _getkey (void) 
{
	char c;
	if(RX.buffer_length)
	{													//Se o buffer tem caracteres
		EIE2 &= ~(1<<ES1);                    								//Desativa a interrupcao serial       
		c = RX.buffer[RX.start++ & (LEN-1)];								//Le o caracter do buffer
		EIE2 |= (1<<ES1);  										//Ativa as interrupcao serial
		RX.buffer_length--;										//Diminui o tamanho do buffer
		return (c);
	}
	return '0';
}

// ********************************************** 
// **	        get_RXactive() function	       **
// **********************************************

bit try_receive_message()
{														//Verifica a existencia de \n e consequentemente uma keyword valida
	char i;
	if(RX.buffer_length >= LEN)										//Se esta cheio de lixo
		Init_UART();
	for(i = 0; i < RX.buffer_length && RXactive == 0 ; i++)
		RXactive = ~((bit)(RX.buffer[(RX.start + i)  & (LEN-1)] ^ 0x0A));    
	return RXactive;
}


// ********************************************** 
// **	        get_keyword() function	       **
// **********************************************

char get_keyword()
{
    char i = 0;
    bit j = 0;
    byte_integer32_t temp;
    temp.integer32 = 0;
    while(RX.buffer[RX.start & (LEN - 1)] != '\r')								//Ler a keyword
		temp.byte[i++ & 0x03] = _getkey();
	
	_getkey();	
	_getkey();												//Retirar o \r\n do buffer
	RXactive = 0;
	for( i = 0; j == 0 && i < 7; i++)									//Verificar a keyword recebida
		j = ~((bit)(temp.integer32 ^ keyword[i]));			
	return (i - 1);	
}

// ********************************************** 
// **	      get_numberdata() function        **
// **********************************************

char get_numberdata()
{ 
	char i;
	char temp[2];
	temp[0] = temp[1] = 13;                         
							
	while(!try_receive_message());                                                        		 	//Esperar o imput do numero
	for(i = 0;RX.buffer[RX.start & (LEN - 1)] != '\r'; i++)
	{													//Ler o numero
		if(i < 2)
			temp[i] = (_getkey() - 48);
		else
			_getkey();
	}
	_getkey();       
	_getkey();	
	RXactive = 0;	                                                                			//Retirar o \r\n do buffer

	if(temp[1] == 13){                                                              			//Caso seja apenas um digito troca as posiçoes
		temp[1] = temp[0];																												
		temp[0] = 0;																															
	}				
	if(temp[0] < 0 || temp[0] > 9 || temp[1] < 0 || temp[1] > 9 )						//Caso seja um numero invalido
		return -1;				
	return temp[0]*10 + temp[1];										//Retorna o numero
}

