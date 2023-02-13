#ifndef __Global_H
#define __Global_H
#include "stm32f10x.h"

// put procedure header here
extern u32 sheep;
extern u32 ps2count; 
extern u32 timeout;
extern u32 ps2key; //data
extern u32 last_ps2key;
extern int release;
extern u8 SYS_CountDown;
extern int SYS_GameOverUI;
extern int SYS_ULTCasting;
extern u8 SYS_BossAttack;
extern int SYS_ULTCD;
extern int SYS_PharseChange;
extern int SYS_USARTPRINT;
#endif
