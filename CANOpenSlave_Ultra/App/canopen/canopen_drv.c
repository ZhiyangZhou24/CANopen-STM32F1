#include "canopen_drv.h"
#include "bsp_can.h"
#include "bsp_timer.h"
#include "bsp_usart.h"
#include "Slave.h"


/* 静态变量 ------------------------------------------------------------------*/
static xQueueHandle xCANSendQueue = NULL;        //CAN发送队列
static xQueueHandle xCANRcvQueue = NULL;         //CAN接收队列

/* 定时器TIM相关变量 */
static TIMEVAL last_counter_val = 0;
static TIMEVAL elapsed_time = 0;


/* 静态申明 ------------------------------------------------------------------*/
static void CANSend_Task(void *pvParameters);
static void CANRcv_Task(void *pvParameters);


/************************************************
函数名称 ： CANOpen_Driver_Init
功    能 ： CANOpen驱动初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void CANOpen_Driver_Init(void)
{
  BaseType_t xReturn;

  /* 创建队列 */
  if(xCANSendQueue == NULL)
  {
    xCANSendQueue = xQueueCreate(CANTX_QUEUE_LEN, sizeof(CanTxMsg));
    if(xCANSendQueue == NULL)
    {
      printf("CANSendQueue create failed");
      return;                                    //创建发送队列失败
    }
  }

  if(xCANRcvQueue == NULL)
  {
    xCANRcvQueue = xQueueCreate(CANRX_QUEUE_LEN, sizeof(CanRxMsg));
    if(xCANRcvQueue == NULL)
    {
      printf("CANRcvQueue create failed");
      return;                                    //创建接收队列失败
    }
  }

  /* 创建任务 */
  xReturn = xTaskCreate(CANSend_Task, "CANSend_Task", CANTX_STACK_SIZE, NULL, CANTX_TASK_PRIORITY, NULL);
  if(pdPASS != xReturn)
  {
    printf("CANSend_Task create failed");
    return;                                      //创建发送任务失败
  }

  xReturn = xTaskCreate(CANRcv_Task, "CANRcv_Task", CANRX_STACK_SIZE, NULL, CANRX_TASK_PRIORITY, NULL);
  if(pdPASS != xReturn)
  {
    printf("CANRcv_Task create failed");
    return;                                      //创建接收任务失败
  }
}

/************************************************
函数名称 ： CANSend_Date
功    能 ： CAN发送数据
参    数 ： TxMsg --- 发送(CAN)消息
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void CANSend_Date(CanTxMsg TxMsg)
{
  if(xQueueSend(xCANSendQueue, &TxMsg, 100) != pdPASS)
  {                                              //加入队列失败
    printf("CANSendQueue failed");
  }
}

/************************************************
函数名称 ： CANSend_Task
功    能 ： CAN发送应用任务程序
参    数 ： pvParameters --- 可选参数
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
static void CANSend_Task(void *pvParameters)
{
  static CanTxMsg TxMsg;

  for(;;)
  {
    /* 等待接收有效数据包 */
    if(xQueueReceive(xCANSendQueue, &TxMsg, 100) == pdTRUE)
    {
      if(CAN_Transmit(CANx, &TxMsg) == CAN_NO_MB)
      {
        vTaskDelay(1);                           //第一次发送失败, 延时1个滴答, 再次发送
        CAN_Transmit(CANx, &TxMsg);
      }
    }
  }
}

/************************************************
函数名称 ： CANRcv_DateFromISR
功    能 ： CAN接收数据
参    数 ： RxMsg --- 接收数据(队列)
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void CANRcv_DateFromISR(CanRxMsg *RxMsg)
{
  static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  if(NULL != xCANRcvQueue)
  {
    xQueueSendFromISR(xCANRcvQueue, RxMsg, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
  }
}

/************************************************
函数名称 ： CANRcv_Task
功    能 ： CAN接收应用任务程序
参    数 ： pvParameters --- 可选参数
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
static void CANRcv_Task(void *pvParameters)
{
  static CanRxMsg RxMsg;
  static Message msg;

  uint8_t i = 0;

  for(;;)
  {
    if(xQueueReceive(xCANRcvQueue, &RxMsg, 100) == pdTRUE)
    {
      msg.cob_id = RxMsg.StdId;                  //CAN-ID

      if(CAN_RTR_REMOTE == RxMsg.RTR)
        msg.rtr = 1;                             //远程帧
      else
        msg.rtr = 0;                             //数据帧

      msg.len = (UNS8)RxMsg.DLC;                 //长度

      for(i=0; i<RxMsg.DLC; i++)                 //数据
        msg.data[i] = RxMsg.Data[i];

      TIM_ITConfig(CANOPEN_TIMx, TIM_IT_Update, DISABLE);
      canDispatch(&Slave_Data, &msg);        //调用协议相关接口
      TIM_ITConfig(CANOPEN_TIMx, TIM_IT_Update, ENABLE);
    }
  }
}

/********************************** CANOpen接口函数(需自己实现) **********************************/
/************************************************
函数名称 ： canSend
功    能 ： CAN发送
参    数 ： notused --- 未使用参数
            m --------- 消息参数
返 回 值 ： 0:失败  1:成功
作    者 ： SWUST Tom
*************************************************/
unsigned char canSend(CAN_PORT notused, Message *m)
{
  uint8_t i;
  static CanTxMsg TxMsg;
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  TxMsg.StdId = m->cob_id;

  if(m->rtr)
      TxMsg.RTR = CAN_RTR_REMOTE;
  else
      TxMsg.RTR = CAN_RTR_DATA;

  TxMsg.IDE = CAN_ID_STD;
  TxMsg.DLC = m->len;
  for(i=0; i<m->len; i++)
      TxMsg.Data[i] = m->data[i];
	
  /* 判断是否在执行中断 */
  if(0 == __get_CONTROL())
  {
    if(xQueueSendFromISR(xCANSendQueue, &TxMsg, &xHigherPriorityTaskWoken) != pdPASS)
    {                                            //加入队列失败
      return 0xFF;
    }
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
  }
  else
  {
    if(xQueueSend(xCANSendQueue, &TxMsg, 100) != pdPASS)
    {                                            //加入队列失败
      return 0xFF;
    }
  }
  return 0;
}

/************************************************
函数名称 ： setTimer
功    能 ： Set the timer for the next alarm.
参    数 ： value --- 参数
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void setTimer(TIMEVAL value)
{
//  uint32_t timer = TIM_GetCounter(CANOPEN_TIMx); // Copy the value of the running timer

//  elapsed_time += timer - last_counter_val;
//  last_counter_val = CANOPEN_TIM_PERIOD - value;
//  TIM_SetCounter(CANOPEN_TIMx, CANOPEN_TIM_PERIOD - value);
//  TIM_Cmd(CANOPEN_TIMx, ENABLE);
//	
	TIM_SetAutoreload(CANOPEN_TIMx, value);// 测试一下这个函数能不能成
}

/************************************************
函数名称 ： getElapsedTime
功    能 ： Return the elapsed time to tell the Stack how much time is spent since last call.
参    数 ： 无
返 回 值 ： (消逝的)时间
作    者 ： SWUST Tom
*************************************************/
TIMEVAL getElapsedTime(void)
{
  uint32_t timer = TIM_GetCounter(CANOPEN_TIMx); // Copy the value of the running timer

  if(timer < last_counter_val)
    timer += CANOPEN_TIM_PERIOD;

  TIMEVAL elapsed = timer - last_counter_val + elapsed_time;

  return elapsed;
}

/************************************************
函数名称 ： TIMx_DispatchFromISR
功    能 ： 定时调度(从定时器中断)
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void TIMx_DispatchFromISR(void)
{
  last_counter_val = 0;
  elapsed_time = 0;
  TimeDispatch();
}
