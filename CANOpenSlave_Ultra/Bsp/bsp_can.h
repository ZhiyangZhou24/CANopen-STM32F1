#ifndef _BSP_CAN_H
#define _BSP_CAN_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f10x.h"


/* 宏定义 --------------------------------------------------------------------*/
#if 1 /* 主站板卡定义 */
#define CANx                      CAN1
#define CAN_CLK                   RCC_APB1Periph_CAN1
#define CAN_GPIO_CLK              RCC_APB2Periph_GPIOB
#define CAN_RX_PIN                GPIO_Pin_8
#define CAN_TX_PIN                GPIO_Pin_9
#define CAN_GPIO_PORT             GPIOB                    //同一个PORT
#define CAN_RX_IRQn               USB_LP_CAN1_RX0_IRQn
#define CAN_RX_Priority           10                       //中断函数接口(见stm32f10x_it.c)
#define CAN_RX_IRQHandler         USB_LP_CAN1_RX0_IRQHandler
#else /* 从站板卡定义 */
#define CANx                      CAN1
#define CAN_CLK                   RCC_APB1Periph_CAN1
#define CAN_GPIO_CLK              RCC_APB2Periph_GPIOA
#define CAN_RX_PIN                GPIO_Pin_11
#define CAN_TX_PIN                GPIO_Pin_12
#define CAN_GPIO_PORT             GPIOA                    //同一个PORT
#define CAN_RX_IRQn               USB_LP_CAN1_RX0_IRQn
#define CAN_RX_Priority           10                       //中断函数接口(见stm32f10x_it.c)
#define CAN_RX_IRQHandler         USB_LP_CAN1_RX0_IRQHandler
#endif


/* 函数申明 ------------------------------------------------------------------*/
void CAN_Initializes(void);


#endif /* _BSP_CAN_H */
