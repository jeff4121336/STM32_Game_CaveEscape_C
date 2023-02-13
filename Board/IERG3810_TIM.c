#include "stm32f10x.h"
#include "IERG3810_TIM.h"

u16 arr = 0;
u16 psc = 0;

void IERG3810_TIM3_PwmInit(u16 arr, u16 psc) {
	RCC->APB2ENR |= 1 << 3;			//lab-1
	GPIOB->CRL &= 0xFF0FFFFF;		//lab-1
	GPIOB->CRL &= 0x00B00000;		//lab-1
	RCC->APB2ENR |= 1 << 0;			//lab-1
	AFIO->MAPR &= 0XFFFFF3FF;		//RM0008 page-184
	AFIO->MAPR |= 1 << 11;
	RCC->APB1ENR |= 1 << 1;			//RM0008 page-115
	TIM3->ARR = arr;					//RM0008 page-419
	TIM3->PSC = psc;					//RM0008 page-418
	TIM3->CCMR1 |= 7 << 12;			//RM0008 page-419
	TIM3->CCMR1 |= 1 << 11;
	TIM3->CCER |= 1 << 4;				//RM0008 page-417
	TIM3->CR1 = 0x0080;				//RM0008 page-404
	TIM3->CR1 |= 0x01;
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & 1<<0)        //check UIF, RM0008 (410)
    {
        GPIOB->ODR ^= 1<<5;     //toggle DS0 with read-modify-write
    }
    TIM3->SR &= ~(1<<0);        //clear UIF, RM0008 (410)
}

// Step 5 ADD this for DS1
void IERG3810_TIM4_Init(u16 arr,u16 psc) 
{
	//TIM4, IRQ#30
	RCC->APB1ENR |= 1<<2;				//RM0008 page-115, TIM4
	TIM4->ARR = arr;						//RM0008 page-419 auto reloaded register
	TIM4->PSC = psc;						//RM0008 page-418 presclar
	TIM4->DIER |= 1<<0;				  //Update interrupt enable/disable 1/0
	TIM4->CR1 |= 0x01;					//Enable/Disable bit 1/0
	NVIC->IP[30] = 0x45;				//start_stm32f10x_hd.s
	NVIC->ISER[0] |= (1<<30);		//start_stm32f10x_hd.s
}

void TIM4_IRQHandler(void)
{
    if (TIM4->SR & 1<<0)        //check UIF, RM0008 (410)
    {
        GPIOE->ODR ^= 1<<5;     //toggle DS1 with read-modify-write
    }
    TIM4->SR &= ~(1<<0);        //clear UIF, RM0008 (410)
}
