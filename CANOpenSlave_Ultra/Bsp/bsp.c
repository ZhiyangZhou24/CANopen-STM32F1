#include "bsp.h"
#include "bsp_can.h"
#include "bsp_timer.h"
#include "bsp_usart.h"


/************************************************
函数名称 ： LED_Initializes
功    能 ： LED初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void LED_Initializes(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);     //开启时钟
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;                                 //推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOC,&GPIO_InitStruct);                                            //初始化IO
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
 // RLED_ON();
}


/************************************************
函数名称 ： BSP_Init
功    能 ： 底层驱动初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void BSP_Init(void)
{
  LED_Initializes();                             //LED底层初始化

  CAN_Initializes();                             //CAN底层初始化
  TIM_Initializes();                             //TIM底层初始化
  USART_Initializes();                           //USART底层初始化
}
