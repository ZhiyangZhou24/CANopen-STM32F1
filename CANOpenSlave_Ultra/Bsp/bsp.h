#ifndef _BSP_H
#define _BSP_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f10x.h"


/* 宏定义 --------------------------------------------------------------------*/
#define LED_GPIO_CLK              RCC_APB2Periph_GPIOC
#define RLED_PIN                  GPIO_Pin_13
#define BLED_PIN                  GPIO_Pin_12
#define LED_GPIO_PORT             GPIOC

/* LED开关 */
#define RLED_ON()                  GPIO_SetBitsGPIO_ResetBits(LED_GPIO_PORT, RLED_PIN)
#define RLED_OFF()                 GPIO_SetBits(LED_GPIO_PORT, RLED_PIN)
#define RLED_TOGGLE()              (LED_GPIO_PORT->ODR ^= RLED_PIN)
#define RLED(n)                    n?GPIO_SetBits(LED_GPIO_PORT, RLED_PIN):GPIO_ResetBits(LED_GPIO_PORT, RLED_PIN)

#define BLED_ON()                  GPIO_SetBits(LED_GPIO_PORT, BLED_PIN)
#define BLED_OFF()                 GPIO_ResetBits(LED_GPIO_PORT, BLED_PIN)
#define BLED_TOGGLE()              (LED_GPIO_PORT->ODR ^= BLED_PIN)

/* 函数申明 ------------------------------------------------------------------*/
void BSP_Init(void);


#endif /* _BSP_H */

