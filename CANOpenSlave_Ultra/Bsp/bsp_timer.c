#include "bsp_timer.h"


/****************************************** CANOpen定时 ******************************************/
/************************************************
函数名称 ： CANOpen_TIM_Configuration
功    能 ： CANOpen定时配置
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void CANOpen_TIM_Configuration(void)
{
  NVIC_InitTypeDef        NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  /* 时钟配置 */
  RCC_APB1PeriphClockCmd(CANOPEN_TIM_CLK, ENABLE);

  /* NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = CANOPEN_TIM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CANOPEN_TIM_Priority;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* 时基配置 */
  TIM_TimeBaseStructure.TIM_Prescaler = CANOPEN_TIM_PRESCALER_VALUE; //预分频值
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //向上计数模式
  TIM_TimeBaseStructure.TIM_Period = CANOPEN_TIM_PERIOD;             //最大计数值(周期)
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //时钟分频因子
  TIM_TimeBaseInit(CANOPEN_TIMx, &TIM_TimeBaseStructure);

  /* 使能中断 */
  TIM_ClearFlag(CANOPEN_TIMx, TIM_IT_Update);                        //清除标志
  TIM_ITConfig(CANOPEN_TIMx, TIM_IT_Update, ENABLE);                 //使能 TIMx 更新中断

  /* 初始化 */
  TIM_SetCounter(CANOPEN_TIMx, 0);
  TIM_Cmd(CANOPEN_TIMx, ENABLE);
}

/************************************************
函数名称 ： TIM_Initializes
功    能 ： TIM初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void TIM_Initializes(void)
{
  CANOpen_TIM_Configuration();
}

