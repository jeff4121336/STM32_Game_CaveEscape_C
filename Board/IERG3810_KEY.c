#include "stm32f10x.h"
#include "IERG3810_KEY.h"

// put your procedure and code here

void IERG3810_KEY_Init(void) 
{
		RCC ->APB2ENR |= 1 << 6; //Enable Port E (LED DS1 / Key1 / Key2)
	  GPIOE->CRL &= 0xFFFF0FFF; //Key1 (PE3)
		GPIOE->CRL |= 0x00008000; 
		
		GPIOE->CRL &= 0xFFFFF0FF; //Key2 (PE2)
		GPIOE->CRL |= 0x00000800; 
		
		RCC ->APB2ENR |= 1 << 2; //Enable Port A (KEY_UP)
		GPIOA->CRL &= 0xFFFFFFF0; //KEY_UP (PA0)
		GPIOA->CRL |= 0x00000008; 

		GPIOE->BSRR = 1 << 2; // Key 2 *starting status*
		GPIOE->BSRR = 1 << 3; // Key 3 *starting status*
		GPIOB->BRR = 1; // KeyUP *starting status*
}
