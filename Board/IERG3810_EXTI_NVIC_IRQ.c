#include "stm32f10x.h"
#include "IERG3810_EXTI_NVIC_IRQ.h"
#include "IERG3810_LED.h"
#include "IERG3810_Clock.h"
// put your procedure and code here


u32 ps2key = 0; //data
u32 last_ps2key = 0;
int release = 0;
u32 sheep = 0;
u32 ps2count = 0; 
u32 timeout = 20000;

void delay(u32 count) 
{
	u32 i;
	for (i=0; i<count; i++);
}

void InputSetup()
{
		IERG3810_clock_tree_init();
    IERG3810_LED_Init();
    IERG3810_NVIC_SetPriorityGroup(5);
    //IERG3810_key2_ExtiInit(); 
    //IERG3810_keyUP_ExtiInit();
    IERG3810_ps2key_ExtiInit();
}

void IERG3810_ps2key_ExtiInit(void)  //ps2key PC11
{
    RCC -> APB2ENR |= 1 << 4; //Enable Port C 
    GPIOC -> CRH &= 0xFFFF00FF;  //modify PC10, 11 CRH changed
    GPIOC -> CRH |= 0x00008800; //pull high/low and input mode -> "1000" CRH change (not sure)
		GPIOE -> ODR |= 1 << 11;
    RCC -> APB2ENR |= 0x01; //Enable AFIO clock (p146)
    AFIO -> EXTICR[2] &= 0xFFFF00FF; // AFIP_EXTICR1 (p191) changed
    AFIO -> EXTICR[2] |= 0x00002200; // changed
    EXTI -> IMR |= 1 << 11;  //edge trigger (p211) changed
    EXTI -> FTSR |= 1 << 11; //falling edge (p212) changed
       
		NVIC -> IP[40] = 0x020; //priority of this interrupt (KeyUP)
    NVIC -> ISER[1] &= ~(1 << 8); //set NVIC 'SET ENABLE REGISTER' 
                                  //DDI0337E page-8-3
    NVIC -> ISER[1] |= (1 << 8);  //IRQ 32-63 
}

void IERG3810_keyUP_ExtiInit(void)  //KEY UP 
{
    RCC -> APB2ENR |= 1 << 2; //Enable Port A 
    GPIOA -> CRL &= 0xFFFFFFF0;  //modify PA0 
    GPIOA -> CRL |= 0x00000008; //pull high/low and input mode -> "1000"
    //GPIOA -> ODR |= 0;

    RCC -> APB2ENR |= 0x01; //Enable AFIO clock (p146)
    AFIO -> EXTICR[0] &= 0xFFFFFFF0; // AFIP_EXTICR1 (p191) 
    AFIO -> EXTICR[0] |= 0x00000000; // need to check
    EXTI -> IMR |= 1;  //edge trigger (p211)
    EXTI -> RTSR |= 1; //rising edge (p212)
    
    NVIC -> IP[6] = 0x35; //priority of this interrupt (KeyUP)
    NVIC -> ISER[0] &= ~(1 << 6); //set NVIC 'SET ENABLE REGISTER'
                                  //DDI0337E page-8-3
    NVIC -> ISER[0] |= (1 << 6);  //IRQ 
}

void IERG3810_key2_ExtiInit(void)
{  
    RCC -> APB2ENR |= 1 << 6; //Enable port-E (KEY2)
    GPIOE -> CRL &= 0xFFFFF0FF; //modify PE2
    GPIOE -> CRL |= 0x00000800; //pull high/low and input mode -> "1000"
    GPIOE -> ODR |= 1 << 2; //pull high
	
    RCC -> APB2ENR |= 0x01; //Enable AFIO clock (p146)
    AFIO -> EXTICR[0] &= 0xFFFFF0FF; // AFIP_EXTICR1 (p191)
    AFIO -> EXTICR[0] |= 0x00000400;
    EXTI -> IMR |= 1 << 2;  //edge trigger (p211)
    EXTI -> FTSR |= 1 << 2; //falling edge (p212)
    //EXTI -> RTSR |= 1 << 2; //rising edge (p212)	
    NVIC -> IP[8] = 0x65; //set priority of this interrupt
    NVIC -> ISER[0] &= ~(1 << 8); //set NVIC 'SET ENABLE REGISTER'
                                  //DDI0337E page-8-3
    NVIC -> ISER[0] |= (1 << 8);  //IRQ8
}

void IERG3810_NVIC_SetPriorityGroup(u8 prigroup)
{
    //Set PRIGROUP AIRCR[10:8]
    u32 temp, temp1;
    temp1 = prigroup & 0x00000007; //only concern 3 bits
    temp1 <<= 8; // 'Why?
    temp = SCB -> AIRCR; //ARM DDI0337E page 8-22
    temp &= 0x0000F8FF; //ARM DDI0337E page 8-22
    temp |= 0x05FA0000; //ARM DDI0337E page 8-22
    temp |= temp1;
    SCB -> AIRCR = temp;
}

void EXTI2_IRQHandler(void)
{
    //EXTI2 handler for KEY2//
    u8 i; 
    for (i = 0; i < 10; i++) 
    {
        DS0(1); //DS0_ON
        delay(1000000);
        DS0(0); //DS0_OFF
        delay(1000000);
    }
    EXTI -> PR = 1 << 2; //clear this exception pending bit
}

void EXTI0_IRQHandler(void)
{
    //EXTI0 handler for KEY UP//
    u8 i; 
    for (i = 0; i < 10; i++) 
    {
        DS1(1); //DS0_ON
        delay(1000000);
        DS1(0); //DS0_OFF
        delay(1000000);
    }
    EXTI -> PR = 1; //clear this exception pending bit
}

void EXTI15_10_IRQHandler(void) //EXTI 10 - 15
{
    //EXTI15 handler for KEY UP//
		//save data//
		u8 bit;

		bit = ((GPIOC -> IDR) >> 10 & 0x00000001); // 1 OR 0
		if ( (ps2count <= 8) && (ps2count >= 1) )
		{
				ps2key |= bit << (ps2count - 1); // OR to set it 1 in particular bit
		} 
		ps2count++; //a interrupt
		delay(10);

    EXTI -> PR = 1 << 11; //clear this exception pending bit
}
