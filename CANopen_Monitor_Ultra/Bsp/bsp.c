#include "bsp.h"
#include "bsp_can.h"
#include "bsp_timer.h"
#include "bsp_usart.h"

void LED_Initializes(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);     //开启时钟
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;                                 //推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOE,&GPIO_InitStruct);                                            //初始化IO
	
	GPIO_SetBits(GPIOE,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);
  RLED_ON();
	GLED_ON();
	BLED_ON();
}

void BSP_Init(void)
{
  LED_Initializes();                             //LED底层初始化
  TIM_Initializes();                             //系统时间戳定时器初始化，延时定时器初始化
  
	USART_Initializes();                           //USART底层初始化
	
	Lcd_Initialize();
	Touch_Init();
}
