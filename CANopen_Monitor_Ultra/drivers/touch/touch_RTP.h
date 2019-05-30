

#ifdef  use_resistance_touch_panel
#ifndef  __TOUCH_RTP_H__
#define  __TOUCH_RTP_H__

#include "stm32f10x.h"

// A/D 通道选择命令字和工作寄存器
// #define	CHX 	0xd0 	//通道Y+的选择控制字	
// #define	CHY 	0x90	//通道X+的选择控制字
#define	CHX 	0x90 	//通道Y+的选择控制字	
#define	CHY 	0xd0	//通道X+的选择控制字

#define	 SPI_CLK	  GPIO_Pin_13
#define  SPI_CS		  GPIO_Pin_12
#define	 SPI_MOSI	  GPIO_Pin_15
#define	 SPI_MISO	  GPIO_Pin_14
#define  TP_INT_PIN	  GPIO_Pin_4

#define TP_DCLK(a)	\
						if (a)	\
						GPIO_SetBits(GPIOB,GPIO_Pin_13);	\
						else		\
						GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define TP_CS(a)	\
						if (a)	\
						GPIO_SetBits(GPIOB,GPIO_Pin_12);	\
						else		\
						GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define TP_DIN(a)	\
						if (a)	\
						GPIO_SetBits(GPIOB,GPIO_Pin_15);	\
						else		\
						GPIO_ResetBits(GPIOB,GPIO_Pin_15)

#define TP_DOUT		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//	//数据输入

void Touch_GPIO_Config(void);

int GUI_TOUCH_X_MeasureX(void); 
int GUI_TOUCH_X_MeasureY(void);

void TP_GetAdXY(unsigned int *x,unsigned int *y);

#endif                                     
#endif
