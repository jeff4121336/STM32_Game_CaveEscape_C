#include "stm32f10x.h"
#include "IERG3810_USART.h"

// put your procedure and code here

void IERG3810_USART1_init(u32 pclk2, u32 bound)
{
		float temp;
		u16 mantissa;
		u16 fraction;
	
		temp = (float)(pclk2 * 1000000)/(bound*16);
		mantissa = temp;
		fraction = (temp-mantissa) * 16;
		mantissa <<= 4;
		mantissa += fraction;
	
		RCC -> APB2ENR |= 1 << 2;
		RCC -> APB2ENR |= 1 << 14; 
		GPIOA -> CRH &= 0xfffff00f;
		GPIOA -> CRH |= 0x000008B0; 
		RCC -> APB2RSTR |= 1 << 14;
		RCC -> APB2RSTR &= ~(1 << 14); 
		USART1 -> BRR = mantissa; 
		USART1 -> CR1 |= 0x2008; 
}

void IERG3810_USART2_init(u32 pclk1, u32 bound)
{
		float temp;
		u16 mantissa;
		u16 fraction;

		temp = (float) (pclk1 * 1000000) / (bound * 16);
		mantissa = temp;
		fraction = (temp-mantissa) * 16;
		mantissa <<= 4;
		mantissa += fraction;
		
		RCC -> APB2ENR |= 1 << 2;
		RCC -> APB1ENR |= 1 << 17;
		GPIOA -> CRL &= 0xFFFF00FF;
		GPIOA -> CRL |= 0x00008B00;
		RCC -> APB1RSTR |= 1 << 17;
		RCC -> APB1RSTR &= ~(1 << 17);
		USART2 -> BRR = mantissa;
		USART2 -> CR1 |= 0x2008;
}
