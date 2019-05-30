#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H


/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f10x.h"


/* 宏定义 --------------------------------------------------------------------*/
/* CANOpen定时 */
#define CANOPEN_TIMx              TIM2
#define CANOPEN_TIM_CLK           RCC_APB1Periph_TIM2
#define CANOPEN_TIM_IRQn          TIM2_IRQn
#define CANOPEN_TIM_Priority      12
#define CANOPEN_TIM_IRQ_Handler   TIM2_IRQHandler

#define CANOPEN_TIM_COUNTER_CLOCK           100000         //计数时钟(100K次/秒)
                                                           //预分频值
#define CANOPEN_TIM_PRESCALER_VALUE         (SystemCoreClock/CANOPEN_TIM_COUNTER_CLOCK - 1)
#define CANOPEN_TIM_PERIOD                  65535          //定时周期


/* 函数申明 ------------------------------------------------------------------*/
void TIM_Initializes(void);


#endif /* _BSP_TIMER_H */
