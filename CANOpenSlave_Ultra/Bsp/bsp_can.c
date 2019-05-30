#include "bsp_can.h"


/************************************************
函数名称 ： CAN_GPIO_Configuration
功    能 ： CAN引脚配置
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void CAN_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* 使能时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | CAN_GPIO_CLK, ENABLE);

  /* 引脚配置 */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN;      //Rx
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;      //Tx
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

  if(GPIOB == CAN_GPIO_PORT)
  {
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1 , ENABLE);
//  GPIO_PinRemapConfig(GPIO_Remap2_CAN1 , ENABLE);
  }
}

/************************************************
函数名称 ： CAN_GPIO_Configuration
功    能 ： CAN参数配置
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void CAN_Configuration(void)
{
  NVIC_InitTypeDef      NVIC_InitStructure;
  CAN_InitTypeDef       CAN_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;

  /* 使能时钟 */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

  /* NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CAN_RX_Priority;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* 配置CAN参数 */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

  /* 配置CAN波特率 = 1MBps(36MHz/6/(1+3+2)) */
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
  CAN_InitStructure.CAN_Prescaler = 6;
  CAN_Init(CANx, &CAN_InitStructure);

  /* CAN过滤配置 */
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  /* 使能中断 */
  CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}

/************************************************
函数名称 ： CAN_Initializes
功    能 ： CAN初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void CAN_Initializes(void)
{
  CAN_GPIO_Configuration();
  CAN_Configuration();
}

