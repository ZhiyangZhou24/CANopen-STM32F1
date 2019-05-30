#ifndef _BSP_USART_H
#define _BSP_USART_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>


/* 宏定义 --------------------------------------------------------------------*/
/* DEBUG */
#define DEBUG_COM                 USART1
#define DEBUG_COM_CLK             RCC_APB2Periph_USART1
#define DEBUG_COM_TX_GPIO_CLK     RCC_APB2Periph_GPIOA     //UART TX
#define DEBUG_COM_TX_PIN          GPIO_Pin_9
#define DEBUG_COM_TX_GPIO_PORT    GPIOA
#define DEBUG_COM_RX_GPIO_CLK     RCC_APB2Periph_GPIOA     //UART RX
#define DEBUG_COM_RX_PIN          GPIO_Pin_10
#define DEBUG_COM_RX_GPIO_PORT    GPIOA
#define DEBUG_COM_IRQn            USART1_IRQn
#define DEBUG_COM_Priority        10                       //优先级
#define DEBUG_COM_BaudRate        115200                   //波特率
#define DEBUG_COM_IRQHandler      USART1_IRQHandler        //中断函数接口(见stm32f10x_it.c)


/* 函数申明 ------------------------------------------------------------------*/
void DEBUG_SendByte(uint8_t Data);
void DEBUG_SendNByte(uint8_t *pData, uint16_t Length);

void USART_Initializes(void);


#endif /* _BSP_USART_H */

