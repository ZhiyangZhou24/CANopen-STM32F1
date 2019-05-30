#ifdef  use_capacitive_touch_panel

#ifndef  __TOUCH_CTP_H__
#define  __TOUCH_CTP_H__

#include "stm32f10x.h"

#define FT_SCL_Pin   GPIO_Pin_11
#define FT_SDA_Pin   GPIO_Pin_10  
#define FT_INT_Pin   GPIO_Pin_12   //中断引脚
			
#define I2C1_SCL_OUT(x) 	\
						if (x)	\
						GPIO_SetBits(GPIOG,FT_SCL_Pin);	\
						else		\
						GPIO_ResetBits(GPIOG,FT_SCL_Pin);
						
#define I2C1_SDA_OUT(x)	\
						if (x)	\
						GPIO_SetBits(GPIOG,FT_SDA_Pin);	\
						else		\
						GPIO_ResetBits(GPIOG,FT_SDA_Pin);


#define	I2C1_SDA_IN()   GPIO_ReadInputDataBit(GPIOG,FT_SDA_Pin)
						
						
						
#define FT6206_ADDR		0x70
#define TOUCH_ADD			0x70  //地址为0x38要移一位
						

void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_Ack(void);
void I2C1_NoAck(void);

void I2C1_Send_Byte(uint8_t dat);
uint8_t I2C1_Read_Byte(uint8_t ack);
uint8_t I2C1_WaitAck(void);

void I2C1_Delay_us(u16 cnt);


static uint8_t FT6206_Write_Reg(uint8_t startaddr,uint8_t *pbuf,uint32_t len);
static uint8_t CheckSum(uint8_t *buf);
uint8_t CTP_Read(uint8_t flag);

uint8_t FT6206_Read_Reg(uint8_t *pbuf,uint32_t len);

void Touch_GPIO_Config(void);

int GUI_TOUCH_X_MeasureX(void); 
int GUI_TOUCH_X_MeasureY(void);

void TP_GetAdXY(unsigned int *x,unsigned int *y);




#endif                                     
#endif
