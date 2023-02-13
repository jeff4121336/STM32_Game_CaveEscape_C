#include "stm32f10x.h"
#include "IERG3810_TFTLCD.h"
#include "IERG3810_clock.h"
#include "IERG3810_EXTI_NVIC_IRQ.h"
#include "FONT.H"
#include "CFONT.H"
#include "PLAYER.H"
#include "BOSS.H" 

typedef struct 
{
    u16 LCD_REG;
    u16 LCD_RAM;
} LCD_TypeDef; 

#define LCD_BASE ((u32) (0x6C000000 | 0x000007FE))
#define LCD ((LCD_TypeDef *) LCD_BASE)

void TFTLCD_Setup()
{
		u16 regval;
		u16 data;
		IERG3810_TFTLCD_WrReg(regval);
		IERG3810_TFTLCD_WrData(data);
		LCD_Set9341_Parameter();
		IERG3810_TFTLCD_Init();
		delay(1000000); //For fill rectangle
		IERG3810_TFTLCD_FillRectangle(0x0000,0x0000,0x00F0,0,0x0140);
}

void IERG3810_TFTLCD_WrReg(u16 regval) //Write command to the TFT-LCD
{
    LCD -> LCD_REG = regval;
}

void IERG3810_TFTLCD_WrData(u16 data) //Write data to the TFT-LCD
{
    LCD -> LCD_RAM = data;
}

void LCD_Set9341_Parameter()
{
    IERG3810_TFTLCD_WrReg(0x01); //Reset Software
    IERG3810_TFTLCD_WrReg(0x11); //Exit_sleep_mode

    IERG3810_TFTLCD_WrReg(0x3A); //Set pixel fromat
    IERG3810_TFTLCD_WrData(0x55); //65536 colors

    IERG3810_TFTLCD_WrReg(0x29); //Display On

    IERG3810_TFTLCD_WrReg(0x36); //Memory Accesss Control
    IERG3810_TFTLCD_WrData(0xCA); //Control Display direction
}

void IERG3810_TFTLCD_Init() //Setting FSMC
{
    RCC -> AHBENR |= 1 << 8; 	//FSMC
    RCC -> APB2ENR |= 1 << 3;	//Port B
    RCC -> APB2ENR |= 1 << 5;	//Port D
    RCC -> APB2ENR |= 1 << 6;	//Port E
    RCC -> APB2ENR |= 1 << 8; 	//Port G
	
    /* PB0 */
    GPIOB -> CRL &= 0xFFFFFFF0; 
    GPIOB -> CRL |= 0x00000003;

    /* Port D */
    GPIOD -> CRH &= 0x00FFF000;
    GPIOD -> CRH |= 0xBB000BBB;  
    GPIOD -> CRL &= 0xFF00FF00;
    GPIOD -> CRL |= 0x00BB00BB;

    /* Port E */
    GPIOE -> CRH &= 0x00000000;
    GPIOE -> CRH |= 0xBBBBBBBB;  
    GPIOE -> CRL &= 0x0FFFFFFF;
    GPIOE -> CRL |= 0xB0000000;

    /* Port G */
    GPIOG -> CRH &= 0xFFF0FFFF;
    GPIOG -> CRH |= 0x000B0000;  
    GPIOG -> CRL &= 0xFFFFFFF0; //PG0 -> RS
    GPIOG -> CRL |= 0x0000000B;

    /* Using FSMC Bank 4 memory bank - Mode A */
	
    FSMC_Bank1 -> BTCR[6] = 0x00000000;		//FSMC_BCR4 - reset
    FSMC_Bank1 -> BTCR[7] = 0x00000000;		//FSMC_BTR4 - reset
    FSMC_Bank1E -> BWTR[6] = 0x00000000;	//FSMC_BWTR4 - reset
    FSMC_Bank1 -> BTCR[6] |= 1 << 12;		//FSMC_BCR4 -> WREN
    FSMC_Bank1 -> BTCR[6] |= 1 << 14;		//FSMC_BCR4 -> EXTMOD
    FSMC_Bank1 -> BTCR[6] |= 1 << 4;		//FSMC_BCR4 -> MWID
    FSMC_Bank1 -> BTCR[7] |= 0 << 28;		//FSMC_BTR4 -> ACCMOD
    FSMC_Bank1 -> BTCR[7] |= 1 << 0;		//FSMC_BTR4 -> ADDSET
    FSMC_Bank1 -> BTCR[7] |= 0xF << 8;		//FSMC_BTR4 -> DATAST
    FSMC_Bank1E -> BWTR[6] |= 0 << 28;		//FSMC_BWTR4 -> ACCMOD
    FSMC_Bank1E -> BWTR[6] |= 0 << 0;		//FSMC_BWTR4 -> ADDSET
    FSMC_Bank1E -> BWTR[6] |= 3 << 8;		//FSMC_BWTR4 -> DATAST
    FSMC_Bank1 -> BTCR[6] |= 1 << 0;		//FSMC_BCR4 -> FACCEN
    LCD_Set9341_Parameter(); 			// set9341
    GPIOB->BSRR |= 0x00000001;     		//LED_LIGHT_ON;
}

void IERG3810_TFTLCD_DrawDot(u16 x, u16 y, u16 color)
{
    IERG3810_TFTLCD_WrReg(0x2A);		//Set x position
    IERG3810_TFTLCD_WrData(x >> 8);
    IERG3810_TFTLCD_WrData(x & 0xFF);
    IERG3810_TFTLCD_WrData(0x01);
    IERG3810_TFTLCD_WrData(0x3F);
	
    IERG3810_TFTLCD_WrReg(0x2B);		//Set y position
    IERG3810_TFTLCD_WrData(y >> 8);
    IERG3810_TFTLCD_WrData(y & 0xFF);
    IERG3810_TFTLCD_WrData(0x01);
    IERG3810_TFTLCD_WrData(0xDF);

    IERG3810_TFTLCD_WrReg(0x2C);		//Set point with color
    IERG3810_TFTLCD_WrData(color);
}



void IERG3810_TFTLCD_FillRectangle(u16 color, u16 start_x, 
u16 length_x, u16 start_y, u16 length_y)
{
    u32 index = 0;
    IERG3810_TFTLCD_WrReg(0x2A);
    IERG3810_TFTLCD_WrData(start_x >> 8);
    IERG3810_TFTLCD_WrData(start_x & 0xFF);
    IERG3810_TFTLCD_WrData((length_x + start_x - 1) >> 8);
    IERG3810_TFTLCD_WrData((length_x + start_x - 1) & 0xFF);

    IERG3810_TFTLCD_WrReg(0x2B);
    IERG3810_TFTLCD_WrData(start_y >> 8);
    IERG3810_TFTLCD_WrData(start_y & 0xFF);
    IERG3810_TFTLCD_WrData((length_y + start_y - 1) >> 8);
    IERG3810_TFTLCD_WrData((length_y + start_y - 1) & 0xFF);

    IERG3810_TFTLCD_WrReg(0x2C); //LCD_WriteRAM_Prepare
    for (index = 0; index < length_x * length_y; index++) //change to draw an area instead of line
    {
    	IERG3810_TFTLCD_WrData(color);	
    }
}

void IERG3810_TFTLCD_SevenSegment(u16 color, u16 start_x, u16 start_y, int digit) 
//int seems have a easier understanding than u8
// u8 is UNSIGNED variable, some problem when doing a loop in main() //
{
	switch (digit) //depends on the input digit -> using switch here
	{
		case 0:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x000A, 0x0037); //e
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x004B, 0x0037); //f
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x004B, 0x0037); //b
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037); //c
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0000, 0x000A); //d
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0082, 0x000A); //a
			break;
		case 1:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x004B, 0x0037); //b
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037); //c
			break;
		case 2:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x000A, 0x0037); //e
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x004B, 0x0037); //b
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0041, 0x000A); //g
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0000, 0x000A); //d
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0082, 0x000A); //a
			break;
		case 3:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x004B, 0x0037); //b
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037); //c
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0000, 0x000A); //d
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0082, 0x000A); //a
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0041, 0x000A); //g
			break;
		case 4:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x004B, 0x0037); //b
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037); //c
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0041, 0x000A); //g
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x004B, 0x0037); //f
			break;
		case 5:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x004B, 0x0037); //f
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037); //c
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0000, 0x000A); //d
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0082, 0x000A); //a
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0041, 0x000A); //g
			break;
		case 6:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x000A, 0x0037); //e
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x004B, 0x0037); //f
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037); //c
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0000, 0x000A); //d
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0082, 0x000A); //a
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0041, 0x000A); //g
			break;
		case 7:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x004B, 0x0037); //b
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037); //c
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0082, 0x000A); //a
			break;
		case 8:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x000A, 0x0037); //e
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x004B, 0x0037); //f
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x004B, 0x0037); //b
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037); //c
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0000, 0x000A); //d
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0082, 0x000A); //a
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0041, 0x000A); //g
			break;
		case 9:
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0000, 0x000A, start_y + 0x004B, 0x0037); //f
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x004B, 0x0037); //b
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037); //c
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0000, 0x000A); //d
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0082, 0x000A); //a
			IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0041, 0x000A); //g
			break;
		default: 
			break;
				
	}
}

void IERG3810_TFTLCD_ShowChar(u16 x, u16 y, u8 ascii, u16 color, u16 bgcolor){
	
		u8 i,j;
		u8 index;
		u8 height = 16, length = 8;
		if(ascii < 32 || ascii > 127) return;
		ascii -= 32;

		IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(x >> 8);
		IERG3810_TFTLCD_WrData(x & 0xFF);
		IERG3810_TFTLCD_WrData((length + x - 1) >> 8);
		IERG3810_TFTLCD_WrData((length + x - 1) & 0xFF);
	
		IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(y >> 8);
		IERG3810_TFTLCD_WrData(y & 0xFF);
		IERG3810_TFTLCD_WrData((height + y - 1) >> 8);
		IERG3810_TFTLCD_WrData((height + y - 1) & 0xFF);
	
		IERG3810_TFTLCD_WrReg(0x2C);	//LCD_WriteRAM_Prepare();
	
		for(j = 0; j < height / 8; j++){
		
					for(i = 0; i < height / 2; i++){
			
									for(index = 0; index < length; index++){

									if( (asc2_1608[ascii][index * 2 + 1 - j] >> i) & 0x01)
													IERG3810_TFTLCD_WrData(color);
									else
													IERG3810_TFTLCD_WrData(bgcolor);
				
									}
					}
			}
}

void IERG3810_TFTLCD_ShowPlayer(u16 x, u16 y, u8 input, u16 color, u16 bgcolor){
	
		u8 i,j;
		u8 index;
		u8 height = 16, length = 8;

		IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(x >> 8);
		IERG3810_TFTLCD_WrData(x & 0xFF);
		IERG3810_TFTLCD_WrData((length + x - 1) >> 8);
		IERG3810_TFTLCD_WrData((length + x - 1) & 0xFF);
	
		IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(y >> 8);
		IERG3810_TFTLCD_WrData(y & 0xFF);
		IERG3810_TFTLCD_WrData((height + y - 1) >> 8);
		IERG3810_TFTLCD_WrData((height + y - 1) & 0xFF);
	
		IERG3810_TFTLCD_WrReg(0x2C);	//LCD_WriteRAM_Prepare();
	
		for(j = 0; j < height / 8; j++){
		
					for(i = 0; i < height / 2; i++){
			
									for(index = 0; index < length; index++){

									if( (player_1608[input][index * 2 + 1 - j] >> i) & 0x01)
													IERG3810_TFTLCD_WrData(color);
									else
													IERG3810_TFTLCD_WrData(bgcolor);
				
									}
					}
			}
}

void IERG3810_TFTLCD_ShowBoss(u16 x, u16 y, u8 input, u16 color, u16 bgcolor){
	
		u8 i,j;
		u8 index;
		u8 height = 16, length = 48;

		IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(x >> 8);
		IERG3810_TFTLCD_WrData(x & 0xFF);
		IERG3810_TFTLCD_WrData((length + x - 1) >> 8);
		IERG3810_TFTLCD_WrData((length + x - 1) & 0xFF);
	
		IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(y >> 8);
		IERG3810_TFTLCD_WrData(y & 0xFF);
		IERG3810_TFTLCD_WrData((height + y - 1) >> 8);
		IERG3810_TFTLCD_WrData((height + y - 1) & 0xFF);
	
		IERG3810_TFTLCD_WrReg(0x2C);	//LCD_WriteRAM_Prepare();
	
		for(j = 0; j < height / 8; j++){
		
					for(i = 0; i < height / 2; i++){
			
									for(index = 0; index < length; index++){

									if( (boss_1648[input][index * 2 + 1 - j] >> i) & 0x01)
													IERG3810_TFTLCD_WrData(color);
									else
													IERG3810_TFTLCD_WrData(bgcolor);
				
									}
					}
			}
}

void IERG3810_TFTLCD_ShowChinChar(u16 x, u16 y, u8 input, u16 color, u16 bgcolor){
	
		u8 i,j;
		u8 index;
		u8 height = 16, length = 16;  //16 * 16
		
		//if (input < 0) return;

		IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(x >> 8);
		IERG3810_TFTLCD_WrData(x & 0xFF);
		IERG3810_TFTLCD_WrData((length + x - 1 ) >> 8);
		IERG3810_TFTLCD_WrData((length + x - 1) & 0xFF);
	
		IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(y >> 8);
		IERG3810_TFTLCD_WrData(y & 0xFF);
		IERG3810_TFTLCD_WrData((height + y - 1) >> 8);
		IERG3810_TFTLCD_WrData((height + y - 1) & 0xFF);
	
		IERG3810_TFTLCD_WrReg(0x2C);	//LCD_WriteRAM_Prepare();
	
		for(j = 0; j < height / 8; j++){
		
					for(i = 0; i < height / 2; i++){
			
									for(index = 0; index < length; index++){

									if( (chi_1616[input][index * 2 + 1 - j] >> i) & 0x01)
													IERG3810_TFTLCD_WrData(color);
									else
													IERG3810_TFTLCD_WrData(bgcolor);
				
									}
					}
			}
}
