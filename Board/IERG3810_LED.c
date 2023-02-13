#include "stm32f10x.h"
#include "IERG3810_LED.h"


// put your procedure and code here
void IERG3810_LED_Init(void)
{
		RCC ->APB2ENR |= 1 << 3; //Enable Port B (LED DS0/ BUZZER)
		GPIOB->CRL &= 0xFF0FFFFF; //LED DS0 (PB5) 
		GPIOB->CRL |= 0x00300000;
						
	  	RCC ->APB2ENR |= 1 << 6; //Enable Port E (LED DS1 / Key1 / Key2)
		GPIOE->CRL &= 0xFF0FFFFF; //LED DS1 (PE5)
		GPIOE->CRL |= 0x00300000; 

		GPIOE->BSRR = 1 << 5; // Ensure LED DS1 Unlit
		GPIOB->BSRR = 1 << 5; // Ensure LED DS0 Unlit

}

void DS0(int state)
{
	  RCC ->APB2ENR |= 1 << 3; //Enable Port B (LED DS0/ BUZZER)
	  GPIOB->CRL &= 0xFF0FFFFF; //LED DS0 (PB5) *CRL for bits 0 - 7
    GPIOB->CRL |= 0x00300000;	
	
		if (state == 1) // 1 - TURN ON 
		{
				GPIOB -> BRR = 1 << 5;
		} else
		{
				GPIOB -> BSRR = 1 << 5;
		}
			
}

void DS1(int state)
{
    RCC ->APB2ENR |= 1 << 6; //Enable Port E (LED DS1 / Key1 / Key2)
    GPIOE->CRL &= 0xFF0FFFFF; //LED DS1 (PE5)
    GPIOE->CRL |= 0x00300000;
	
		if (state == 1) // 1 - TURN ON 
		{
				GPIOE -> BRR = 1 << 5;
		} else
		{
				GPIOE -> BSRR = 1 << 5;
		}
}
