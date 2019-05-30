#include "bsp_usart.h"


/***************************************** DEBUG调试串口 *****************************************/
/************************************************
函数名称 ： UART_DEBUG_Configuration
功    能 ： 调试串口配置
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void UART_DEBUG_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;

  /* 时钟配置 */
  RCC_APB2PeriphClockCmd(DEBUG_COM_TX_GPIO_CLK | DEBUG_COM_RX_GPIO_CLK , ENABLE);

  if(USART1 == DEBUG_COM)
    RCC_APB2PeriphClockCmd(DEBUG_COM_CLK, ENABLE);
  else
    RCC_APB1PeriphClockCmd(DEBUG_COM_CLK, ENABLE);

  /* 引脚配置 */
  GPIO_InitStructure.GPIO_Pin = DEBUG_COM_TX_PIN;                    //UART Tx
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(DEBUG_COM_TX_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = DEBUG_COM_RX_PIN;                    //UART Rx
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(DEBUG_COM_RX_GPIO_PORT, &GPIO_InitStructure);

  /* NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_COM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DEBUG_COM_Priority;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* UART配置 */
  USART_InitStructure.USART_BaudRate = DEBUG_COM_BaudRate;           //波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //传输位数
  USART_InitStructure.USART_StopBits = USART_StopBits_1;             //停止位
  USART_InitStructure.USART_Parity = USART_Parity_No ;               //校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //收发功能
  USART_Init(DEBUG_COM, &USART_InitStructure);

  USART_ClearFlag(DEBUG_COM, USART_FLAG_RXNE | USART_FLAG_TC);
  USART_ITConfig(DEBUG_COM, USART_IT_RXNE, ENABLE);                  //接收中断

  USART_Cmd(DEBUG_COM, ENABLE);                                      //使能UART
}

/************************************************
函数名称 ： DEBUG_SendByte
功    能 ： 调试串口发送一个字节
参    数 ： Data --- 数据
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void DEBUG_SendByte(uint8_t Data)
{
  /* 等待发送为空 */
  while(RESET == USART_GetFlagStatus(DEBUG_COM, USART_FLAG_TXE));
  USART_SendData(DEBUG_COM, Data);
}

/************************************************
函数名称 ： DEBUG_SendNByte
功    能 ： 调试串口发送N个字节
参    数 ： pData ---- 数据
            Length --- 长度
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void DEBUG_SendNByte(uint8_t *pData, uint16_t Length)
{
  while(Length--)
  {
    DEBUG_SendByte(*pData);
    pData++;
  }
}

/************************************************
函数名称 ： fputc
功    能 ： 重定义putc函数
参    数 ： ch --- 字符
            *f --- 文件指针
返 回 值 ： 字符
作    者 ： SWUST Tom
*************************************************/
int fputc(int ch, FILE *f)
{
  DEBUG_SendByte((uint8_t)ch);

  return ch;
}

/************************************************
函数名称 ： fgetc
功    能 ： 重定义getc函数
参    数 ： *f --- 文件指针
返 回 值 ： 字符
作    者 ： SWUST Tom
*************************************************/
int fgetc(FILE *f)
{
  while(USART_GetFlagStatus(DEBUG_COM, USART_FLAG_RXNE) == RESET);

  return (int)USART_ReceiveData(DEBUG_COM);
}

/************************************************
函数名称 ： USART_Initializes
功    能 ： 串口初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void USART_Initializes(void)
{
  UART_DEBUG_Configuration();
}

