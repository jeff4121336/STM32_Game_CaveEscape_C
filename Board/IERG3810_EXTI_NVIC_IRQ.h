#ifndef __IERG3810_EXTI_NVIC_IRQ_H
#define __IERG3810_EXTI_NVIC_IRQ_H
#include "stm32f10x.h"
#include "Global.h"
// put procedure header here

void delay(u32);

void IERG3810_ps2key_ExtiInit(void);
void IERG3810_keyUP_ExtiInit(void);
void IERG3810_key2_ExtiInit(void);
void IERG3810_NVIC_SetPriorityGroup(u8);
void EXTI2_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void InputSetup(void);

#endif
