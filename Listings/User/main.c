#include "stm32f10x.h"
#include "stdlib.h"
#include "IERG3810_TFTLCD.h"
#include "IERG3810_LED.h"
#include "IERG3810_clock.h"
#include "IERG3810_EXTI_NVIC_IRQ.h"
#include "Global.h"
#include "IERG3810_TIM.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_USART.h"

#define ClearScreen IERG3810_TFTLCD_FillRectangle(0x0000, 0x0000, 0x00F0, 0x0000, 0x0140);
#define	DrawPlayer	IERG3810_TFTLCD_ShowPlayer(PCoor[0]  * 4, PCoor[1] * 4, 0x0000, 0xFFFF, 0x0000);

#define	DrawBoss1		IERG3810_TFTLCD_ShowBoss(BCoor[BossPosition] * 4, BCoor[3] * 4, 0x0002, 0xFFE0, 0x0000);
#define	DrawBoss2		IERG3810_TFTLCD_ShowBoss(BCoor[BossPosition] * 4, BCoor[4] * 4, 0x0001, 0xFFE0, 0x0000);
#define	DrawBoss3		IERG3810_TFTLCD_ShowBoss(BCoor[BossPosition] * 4, BCoor[5] * 4, 0x0000, 0xFFE0, 0x0000);

#define DrawBossHealth  	IERG3810_TFTLCD_FillRectangle(0xF800, 0x0000, BossHealth / 400, 0x013A, 0x0006); 
#define DrawPlayerHealth  IERG3810_TFTLCD_FillRectangle(0x07E0, 0x0000, PlayerHealth / 5, 0x0126, 0x0006); 
#define DrawBullet(PAttackX, PAttackY) IERG3810_TFTLCD_ShowPlayer(PAttackX, PAttackY, 0x0001, BulletColor, 0x0000);

#define ON0 GPIOB -> BRR = 1 << 5;
#define OFF0 GPIOB -> BSRR = 1 << 5;
#define ON1 GPIOE -> BRR = 1 << 5;
#define OFF1 GPIOE -> BSRR = 1 << 5;
#define ONbuzzer GPIOB -> BSRR = 1 << 8;
#define OFFbuzzer GPIOB -> BRR = 1 << 8;

u8 SYS_CountDown;
u8 SYS_BossAttack;
int SYS_GameOverUI;
int SYS_ULTCasting, SYS_ULTCD , SYS_USARTPRINT, ULT_ENABLE;
int SYS_PharseChange, Pharse;

u16 PCoor[2], BCoor[6]; //(x,y) (x1, x2, x3, y)
int PlayerMap[60][80], BossMap[60][80], ObstacleMap[60][80]; // (x, y) (x, y)
int BossPosition; // 0,1,2
int BossHealth, PlayerHealth;
int endstate, i, j;

u16 BAttackX, BAttackY, PAttackX, PAttackY;
int BossAttackCoorUpdate, AttackType;
int BulletMove, newBullet, BulletDamage;
u16 BulletColor, BossColor;

u16 Boss[4] = {0x42, 0x4F, 0x53, 0x53};
u16 Player[6] = {0x50, 0x4C, 0x41, 0x59, 0x45, 0X52};

// Project //
void InfoSetup()
{
			
			u16 name1[3] = {0x00,0x01,0x02}; //Ascessing CFONT.H
			u16 name2[3] = {0x03,0x04,0x05};
			u16 SID1[10] = {0x31,0x31,0x35,0x35,0x31,0x35,0x38,0x34,0x37,0x37};
			u16 SID2[10] = {0x31,0x31,0x35,0x35,0x31,0x35,0x38,0x36,0x39,0x37};
			u16 GameName[11] = {0x43,0x41,0x56,0x45,0x20,0x45,0x53,0x43,0x41,0x50,0x45};
			u16 row1[19] = {0x50, 0x72, 0x65, 0x73, 0x73, 0x20, 0x4E, 0x55, 0x4D, 0x31, 0x20, 0x74, 0x6F, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74};
			u16 row2[21] = {0x52, 0x45, 0x41, 0x44, 0x20, 0x42, 0x45, 0x46, 0x4F, 0x52, 0x45, 0x20, 0x53, 0x54, 0x41, 0x52, 0x54, 0x20, 0x21, 0x21, 0x21};
			u16 row3[23]  = {0x33, 0x20, 0x70, 0x68, 0x61, 0x73, 0x65, 0x73, 0x20, 0x28, 0x32, 0x35, 0x20, 0x2F, 0x20, 0x32, 0x35, 0x20, 0x2F, 0x20, 0x32, 0x35, 0x29};
			u16 right[13]= {0x4B, 0x65, 0x79, 0x20, 0x36, 0x20, 0x2D, 0x20, 0x72, 0x69, 0x67, 0x68, 0x74};
			u16 left[12] = {0x4B, 0x65, 0x79, 0x20, 0x34, 0x20, 0x2D, 0x20, 0x6C, 0x65, 0x66, 0x74};
			u16 up[10]   = {0x4B, 0x65, 0x79, 0x20, 0x38, 0x20, 0x2D, 0x20, 0x75, 0x70};
			u16 down[12] = {0x4B, 0x65, 0x79, 0x20, 0x35, 0x20, 0x2D, 0x20, 0x64, 0x6F, 0x77, 0x6E};
			u16 ULT[11]  = {0x4B, 0x65, 0x79, 0x20, 0x37, 0x20, 0x2D, 0x20, 0x55, 0x4C, 0x54};
			u16 DS0[18]  = {0x44, 0x53, 0x30, 0x20, 0x75, 0x6C, 0x74, 0x69, 0x6D, 0x61, 0x74, 0x65, 0x20, 0x72, 0x65, 0x61, 0x64, 0x79};
			u16 DS1[28]  = {0x44, 0x53, 0x31, 0x20, 0x76, 0x61, 0x72, 0x69, 0x65, 0x73, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x64, 0x69, 0x66, 0x66, 0x69, 0x63, 0x75, 0x6C, 0x74, 0x69, 0x65, 0x73};
			u16 row9[27]= {0x54, 0x68, 0x65, 0x20, 0x4F, 0x4E, 0x4C, 0x59, 0x20, 0x77, 0x61, 0x79, 0x20, 0x79, 0x6F, 0x75, 0x20, 0x65, 0x73, 0x63, 0x61, 0x70, 0x65, 0x20, 0x74, 0x68, 0x65};
			u16 row10[24]= {0x63, 0x61, 0x76, 0x65, 0x00, 0x69, 0x73, 0x20, 0x74, 0x6F, 0x20, 0x6B, 0x69, 0x6C, 0x6C, 0x20, 0x74, 0x68, 0x65, 0x20, 0x62, 0x6F, 0x73, 0x73};
			u16 row11[18]= {0x77, 0x69, 0x74, 0x68, 0x69, 0x6E, 0x20, 0x37, 0x35, 0x20, 0x73, 0x65, 0x63, 0x6F, 0x6E, 0x64, 0x73};
			


			ClearScreen;
			
			// (320 240) 0x0140 0x00F0
			for(i = 0; i < 12; i++)
						IERG3810_TFTLCD_ShowChar(0x0035 + 0x000D * i, 0x0130, GameName[i], 0xFFFF, 0x0000); //R0
			for(i = 0; i < 3; i++){
						IERG3810_TFTLCD_ShowChinChar(0x0015 + 0x001A * i, 0x0002, name1[i], 0xFFFF, 0x0000); //R12
						IERG3810_TFTLCD_ShowChinChar(0x0015 + 0x001A * i, 0x0019, name2[i], 0xFFFF, 0x0000); //R13
			}
			
			
			for(i = 0; i < 19; i++)
						IERG3810_TFTLCD_ShowChar(0x0015 + 0x000A * i, 0x010D, row1[i], 0xF800, 0x0000); //R1
			for(i = 0; i < 21; i++)
						IERG3810_TFTLCD_ShowChar(0x0020 + 0x0008 * i, 0x00E4, row2[i], 0x1F00, 0x0000);	//R2		
			for(i = 0; i < 23; i++)
						IERG3810_TFTLCD_ShowChar(0x0017 + 0x0008 * i, 0x00D2, row3[i], 0xFFFF, 0x0000);	//R3
			
			
			for(i = 0; i < 13; i++)
						IERG3810_TFTLCD_ShowChar(0x0005 + 0x0008 * i, 0x00C0, right[i], 0xFFFF, 0x0000); //R4		
			for(i = 0; i < 12; i++){
						IERG3810_TFTLCD_ShowChar(0x0085 + 0x0008 * i, 0x00C0, left[i], 0xFFFF, 0x0000); //R4
						IERG3810_TFTLCD_ShowChar(0x0005 + 0x0008 * i, 0x00AC, down[i], 0xFFFF, 0x0000); //R5
			}
			for(i = 0; i < 10; i++)
			{
						IERG3810_TFTLCD_ShowChar(0x0085 + 0x0008 * i, 0x00AC, up[i], 0xFFFF, 0x0000);	 //R5
						IERG3810_TFTLCD_ShowChar(0x0078 + 0x000A * i, 0x0019, SID1[i], 0xFFFF, 0x0000);	//R12
						IERG3810_TFTLCD_ShowChar(0x0078 + 0x000A * i, 0x0002, SID2[i], 0xFFFF, 0x0000);	//R13
			}

			for(i = 0; i < 11; i++)
						IERG3810_TFTLCD_ShowChar(0x0005 + 0x0008 * i, 0x0098, ULT[i], 0xFFFF, 0x0000);	//R6			

			for(i = 0; i < 18; i++)
						IERG3810_TFTLCD_ShowChar(0x0005 + 0x0008 * i, 0x0084, DS0[i], 0xFFFF, 0x0000);	//R7	
			for(i = 0; i < 28; i++)
						IERG3810_TFTLCD_ShowChar(0x0005 + 0x0008 * i, 0x0070, DS1[i], 0xFFFF, 0x0000);  //R8	

			
			for(i = 0; i < 27; i++)
						IERG3810_TFTLCD_ShowChar(0x0008 + 0x0008 * i, 0x0057, row9[i], 0xF800, 0x0000);	//R9
			for(i = 0; i < 24; i++)
						IERG3810_TFTLCD_ShowChar(0x0015 + 0x0008 * i, 0x0048, row10[i], 0xF800, 0x0000);//R10	
			for(i = 0; i < 18; i++)
						IERG3810_TFTLCD_ShowChar(0x0035 + 0x0008 * i, 0x0039, row11[i], 0xF800, 0x0000);//R11
			
}

void USART_print(char *st)
{
		u8 i = 0;
		while (st[i] != 0x00) 
		{
					USART1 -> DR = st[i];
		
					while(( ( (USART1 -> SR) & 0x000000080 ) >> 7 ) != 1);
		
					if (i == 255) break;
								i++;
		}
}

void CoordinatesUpdate()
{
			if (newBullet)
			{
						PAttackX = PCoor[0] * 4;

						newBullet = 0;
			}			
			
			PAttackY = (PCoor[1] + 0x0006) * 4;
			DrawBullet(PAttackX, PAttackY + 0x000C * BulletMove);
			//DrawBullet(PAttackX + 0x0008, PAttackY + 0x0008 * BulletMove);
			
			BulletMove++;
	
			if (BulletMove >= (BCoor[5] - PCoor[1]) * 4 / 0x000C && newBullet == 0)
			{
						BulletMove = 0;
						newBullet = 1;
			}
			
						
			for (i = 0; i < 60; i++)
			{
						for(j = 0; j < 80; j++)
						{
									if (ObstacleMap[i][j] > 0)
									{		
												IERG3810_TFTLCD_ShowChinChar(i * 4, j * 4, 0x0006, 0xF800, 0x0000);			
												ObstacleMap[i][j] -= 1;
									}
						}
			}
			
			for (i = 0; i < 60; i++) 
			{
						for (j = 0; j < 80; j++)
						{	
									if (PlayerMap[i][j] >= 2)
									{			
												PlayerHealth -=  PlayerMap[i][j] * Pharse;
												
									}
									PlayerMap[i][j] = BossMap[i][j] = 0;
						}
			}
			
			for (i = PCoor[0]; i < PCoor[0] + 2; i++)
			{
						for (j = PCoor[1]; j < PCoor[1] + 4; j++)
						{
									PlayerMap[i][j] = 1;					
									if (ObstacleMap[i][j] > 0)
												PlayerHealth -= 2;
						}
			}
			
			for (i = BCoor[BossPosition]; i < BCoor[BossPosition] + 12; i++)
			{
						for (j = 58; j < 58 + 12; j++)
			{
									BossMap[i][j] = 1;
						if (BossMap[PAttackX/4][(PAttackY + 0x000C * BulletMove)/4] == 1)
									BossHealth -= BulletDamage;
			}
			}
}
void FireAttack()
{		
		int P = rand()% 20 ; 	// 0 - 19
		
		//70 % nothing
		if (P < 10) //45%
		{
				int G = rand() % 56;
				for (i = (G % 5); i < 56; i+=7)
						ObstacleMap[i][G] = 60 * Pharse;
		} 
}
	
void BossAttack()
{
		
		if (Pharse == 1)
		{
							IERG3810_TFTLCD_FillRectangle(0xFFE0, BAttackX, 0x0004 , 0x0000, 0x00E8);
							for (j = 0; j < 59; j++)
		 								PlayerMap[(BAttackX)/4][j] += 1;
		}
		
		if (Pharse == 2)
		{
							IERG3810_TFTLCD_FillRectangle(0xFFE0, BAttackX, 0x0006 , 0x0000, 0x00E8);
							IERG3810_TFTLCD_FillRectangle(0xFFE0, 0x0000, 0x00F0 , BAttackY, 0x0006);
							for (j = 0; j < 60; j++)
		 								PlayerMap[(BAttackX)/4][j] += 1;
							for (i = 0; i < 80; i++)
										PlayerMap[i][BAttackY/4] += 1;
		}
		
		if (Pharse == 3)
		{
				//BAttackX = PCoor[0] * 4;
							IERG3810_TFTLCD_FillRectangle(0xF800, BAttackX, 0x0006 , 0x0000, 0x00EE);		
							for (j = 0; j < 59; j++)
		 								PlayerMap[(BAttackX)/4][j] += 1;
		}
}

void GameSetup()
{	
			// map[240][320]; 60 / 80  x,y
			//Set Map Empty
			for (i = 0; i < 60; i++) 
			{
						for (j = 0; j < 80; j++)
						{
									PlayerMap[i][j] = BossMap[i][j] = 0;
						}
			}

			//60 , 42 // 60 , 02 
			//start at 60,22 
			//64 * 64 -> 16 * 16			
			PCoor[0] = 30;
			PCoor[1] = 4;
			
			PlayerMap[30][4] = PlayerMap[30][5] = PlayerMap[30][6] = PlayerMap[30][7] = 1;
			PlayerMap[31][4] = PlayerMap[31][5] = PlayerMap[31][6] = PlayerMap[31][7] = 1;	
			//Set Player Default Start - 1 
			//start at 30,4
			//16 * 8 ->  4 * 2
			for(i = 30; i < 32; i++) 
			{
						for (j = 4; j < 8; j++)
						{
									BossMap[i][j] = 1;
						}
			}
			
			BCoor[0] = 24;
			BCoor[1] = 10;
			BCoor[2] = 38;
						
			BCoor[3] = 58;	
			BCoor[4] = 62;	
			BCoor[5] = 66;	
			
			BossPosition = 0;
			
			for(i = 24; i < 36; i++) 
			{
						for (j = 58; j < 70; j++)
						{
									BossMap[i][j] = 2;
						}			
			}
			
			//another possible places for boss
			
			endstate = 0;
			BossHealth = 96000;
			PlayerHealth = 1200;
			OFF0;
			ULT_ENABLE = 0;
			Pharse = 1;
			BulletMove = 0;
			BossAttackCoorUpdate = 1;
			SYS_CountDown = 0;
			BulletDamage = 1;
			BossColor = 0x7EE0;
			
			
			USART_print("\r\nNarrator: He accidentally bumps into an unknown cave.");
			USART_print("\r\nNarrator: He tries his best to kill the boss and escape from the cave.");
			
			USART_print("\r\nSamson: ???");
			USART_print("\r\nBoss: ARARARARRR");
			
			USART_print("\r\n\r\nGame Start!");
			USART_print("\r\n\r\nPhase 1");
			// Reset Obstacle Map 
			for (i = 0; i < 60; i++) 
						for (j = 0; j < 80; j++)
								ObstacleMap[i][j] = 0;
			
			for (i = 3; i >= 0;) //a loop for digit to count down
			{
						if (SYS_CountDown >= 101)
						{
								IERG3810_TFTLCD_FillRectangle(0xABCD, 0x0000, 0x00F0, 0, 0x0140);
								IERG3810_TFTLCD_SevenSegment(0x0000, 0x0052, 0x0059, i);
								SYS_CountDown = 0;
								i--;
						}
			}
				
			SYS_GameOverUI = 0;
			SYS_BossAttack = 0 ;
			SYS_ULTCD = 0;
			SYS_PharseChange = 0;
}


void InputCheck()
{
		timeout = 12500;
		ps2key = 0;
		ps2count = 0;

				while(1)
				{	
						if (ps2count >= 11)
						{
									if (ps2key == 0x69) 
												break;

						}
						EXTI -> PR = 1 << 11; //reset
						timeout--;
			
						if (timeout == 0) //clear ps2 keyboard data when timeout
						{
									timeout = 20000;
									ps2key = 0;
									ps2count = 0;
						}	 // end of clear ps2 keyboard data when timeout
				}	
}

void IERG3810_SYSTICK_Init10ms(void){
	//SYSTICK
	SysTick->CTRL = 0; //Clear
	SysTick->LOAD = 89999; // 72 000 000 / (8*100) - 1  setting time 100ms -> 10 Hz)
	SysTick->CTRL |= 0x03; // ENABLE TICKINT CLKSOURCE
}

// Lose/Win -> Restart Game UI -> Start Game UI //
void GameOver(u8 state)
{
		u16 Lose[10] = {0x47, 0x41, 0x4D, 0x45, 0x20, 0x4F, 0x56, 0x45, 0x52, 0x21};
		u16 Win[8] = {0x59, 0x6F, 0x75, 0x20, 0x57, 0x69, 0x6E, 0x21};
		u16 instruction[23] = {0x50, 0x72, 0x65, 0x73, 0x73, 0x20, 0x4E, 0x75, 0x6D, 0x31, 0x20, 0x62, 0x61, 0x63, 0x6B, 0x20, 0x74, 0x6F, 0x20, 0x6D, 0x65, 0x6E, 0x75};
		//u16 DS1[23] = {0x44, 0x53, 0x31, 0x20, 0x42, 0x6F, 0x73, 0x73, 0x20, 0x72, 0x65, 0x61, 0x64, 0x79, 0x20, 0x63, 0x61, 0x73, 0x74, 0x20, 0x55, 0x4C, 0x54};


		// RESTART GAME UI HERE //
		OFFbuzzer;
		ClearScreen;
		if (state){
				for (i = 0; i < 8; i++)
							IERG3810_TFTLCD_ShowChar(0x001F + 0x0008 * i, 0x0070, Win[i], 0xFFFF, 0x0000); 
		}
		else {
				for (i = 0; i < 10; i++)
							IERG3810_TFTLCD_ShowChar(0x001F + 0x0008 * i, 0x0070, Lose[i], 0xFFFF, 0x0000); 			
		}			
	
		for (i = 0; i < 23; i++)
				IERG3810_TFTLCD_ShowChar(0x0005 + 0x0008 * i, 0x0060, instruction[i], 0xFFFF, 0x0000); 							
		InputCheck(); //0x69
		
		//FillRectangle;
		//print all char
		TFTLCD_Setup(); // IERG3810_TFTLCD.c - print label
		InfoSetup(); //main.c 
		InputCheck(); //0x69
		GameSetup();
		//Input_Setup(); //IERG3810_EXTI_NVIC_IRQ.c - enable reading ps2keyboard
		//IERG3810_SYSTICK_Init10ms();
}	

void GameLoad()
{
			ClearScreen;
			for (i = 0; i < 4; i++)
			{
						IERG3810_TFTLCD_ShowChar(0x0000 + 0x000D * i, 0x012B, Boss[i], 0xF800, 0x0000);
			}
			for (i = 0; i < 6; i++)
			{
						IERG3810_TFTLCD_ShowChar(0x0000 + 0x000D * i, 0x0118, Player[i], 0x07E0, 0x0000);
			}
			DrawPlayer; //player
			DrawBoss1; //boss
			DrawBoss2;
			DrawBoss3;	
			CoordinatesUpdate();
		
			if (BossHealth < 0)
			{
						GameOver(1);
			} else if ( Pharse >= 4 || PlayerHealth < 0)
			{
						GameOver(0);
			} else
			{
						DrawBossHealth;
						DrawPlayerHealth;
			}
			
}

int main(void) {
		
		u8 sheep;
		TFTLCD_Setup(); // IERG3810_TFTLCD.c - print label
		IERG3810_clock_tree_init();
		IERG3810_LED_Init();
		IERG3810_Buzzer_Init();
	
		InfoSetup();
		InputSetup(); //IERG3810_EXTI_NVIC_IRQ.c - enable reading ps2keyboard
		InputCheck(); //0x69 to start game
		IERG3810_SYSTICK_Init10ms();
			
		IERG3810_USART1_init(72, 9600);

	
		GameSetup(); //main.c

			
		while(1) 
		{
				sheep++;	
				if (SYS_ULTCD > 801) 
				{
							ULT_ENABLE = 1;
							ON0;
				}		
				GameLoad();

				if (SYS_BossAttack > 181) 
				{
							if (BossAttackCoorUpdate)
							{
										BAttackX = PCoor[0] * 4;
										BAttackY = PCoor[1] * 4;
										BossAttackCoorUpdate = 0;
							}
							
							if (Pharse >= 2)
							{			
										if(SYS_BossAttack % 5 == 0)
														FireAttack();
							}
							
							if (Pharse >= 3)
							{
										BossColor = 0xF800;
										if(SYS_BossAttack % 2 == 0)
														BossAttack();
										if(SYS_BossAttack % 8 == 0)
										{
														BossPosition = rand() % 3;
														BossAttackCoorUpdate = 1;
										}
							} else 
							{
										if (SYS_BossAttack % 4 == 0)
														BossAttack();
										if (SYS_BossAttack % 12 == 0)
										{
														BossPosition = rand() % 3;
														BossAttackCoorUpdate = 1;
										}
							}
							
							if (SYS_BossAttack > 301)
							{
										SYS_BossAttack = 0;
							}
				}
				
				if (SYS_ULTCasting <= 0)
				{
							BulletDamage = 1;
							BulletColor = 0xFFFF;
				}
				
				if (SYS_PharseChange > 2501) //25 sec
				{
							ONbuzzer; 
				}
				
				if (SYS_PharseChange > 2551) //buzzer on half sec
				{
							Pharse += 1;
							
							switch (Pharse) 
							{
									case 2:
											USART_print("Pharse 2\n");
											break;
									case 3:
											USART_print("Pharse 3");								
											break;
							}
							
							SYS_PharseChange = 0;
							OFFbuzzer;
				}
				
				if (Pharse == 4 && SYS_GameOverUI > 7901) //79 seconds second buffer 
							GameOver(1);
				//TIM3->CCR2 = SYS_ULTCD * 10;
				//6C(ULT)		75(U)
				//6B(L) 		73(D) 		74(R) 
				if (ps2count >= 11)
				{
							switch (ps2key) { // Range: x - 2 to 58 and y - 2 to 58
									case 0x6B:
											if (PCoor[0] - 0x0003 >= 0) //8 for a step and 6 for healthbar
														PCoor[0] -= 0x0003;
														GameLoad();
											break;
									case 0x73:
											if (PCoor[1] - 0x000B >= 0)
														PCoor[1] -= 0x0003;
														GameLoad();
											break;
									case 0x74:
											if (PCoor[0] + 0x0003 < 0x003C) // 8 for a step and 8 for player size
														PCoor[0] += 0x0003;
														GameLoad();
											break;
									case 0x75:	
											if (PCoor[1] + 0x0021 <= 0x0050) //8 for a step and 16 for player size and 6 for healthbar and 100 for boss area
														PCoor[1] += 0x0003;
														GameLoad();
											break;
									case 0x6C:
											if (ULT_ENABLE == 1)
											{											
														ULT_ENABLE = 0;
														SYS_ULTCD = 0;
														BulletDamage = 3;
														OFF0;
														SYS_ULTCasting = 351;
														BulletColor = 0xF800;
											}				
							}
							
							timeout = 12500;
							ps2key = 0;
							ps2count = 0;
				
				}
				
		}
		
}
