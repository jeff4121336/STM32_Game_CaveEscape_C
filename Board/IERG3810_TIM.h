#ifndef __IERG3810_TIM_H
#define __IERG3810_TIM_H
#include "stm32f10x.h"

void IERG3810_TIM3_PwmInit(u16, u16);
void IERG3810_TIM4_Init(u16,u16);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);

#endif
