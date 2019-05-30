#ifndef  __TOUCH_H__
#define  __TOUCH_H__
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"


#include "stdint.h"
#define FT6206_ADDR		0x70


#define FT_SCL_Pin   GPIO_Pin_11
#define FT_SDA_Pin   GPIO_Pin_10  
#define FT_INT_Pin   GPIO_Pin_12   //ÖÐ¶ÏÒý½Å
			
#define Touch_IIC_SCL(x) 	\
						if (x)	\
						GPIO_SetBits(GPIOG,FT_SCL_Pin);	\
						else		\
						GPIO_ResetBits(GPIOG,FT_SCL_Pin);
						
#define Touch_IIC_SDA(x)	\
						if (x)	\
						GPIO_SetBits(GPIOG,FT_SDA_Pin);	\
						else		\
						GPIO_ResetBits(GPIOG,FT_SDA_Pin);


#define Touch_SDA_IN()  SDA_Input_Mode()
#define Touch_SDA_OUT() SDA_Output_Mode()		

#define Touch_READ_SDA GPIO_ReadInputDataBit( GPIOG, FT_SDA_Pin)
extern int   Pointx1,Pointy1;
extern unsigned int  Data_is_ready;
extern float TP_X,TP_Y;
//extern bool Up_or_Down;

void Touch_Init(void);
unsigned char touch(void);
int Get_TOUCH_X_MeasureY(void);
int Get_TOUCH_X_MeasureX(void);
void SDA_Input_Mode(void);
void SDA_Output_Mode(void);

#endif                                     
