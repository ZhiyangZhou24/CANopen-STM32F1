#include "canopen_drv.h"
#include "canopen_app.h"
#include "Master.h"

/* 定时器TIM相关变量 */
static TIMEVAL last_counter_val = 0;
static TIMEVAL elapsed_time = 0;

void CANOpen_Driver_Init(void)
{
	CANOpen_TIM_Configuration();
	CAN_Initializes();
	CANOpen_Node_Init();
}


void CANSend_Date(CanTxMsg TxMsg)
{
	//if(CAN_Transmit(CANx, &TxMsg) == CAN_NO_MB)
	//{
	//	delay_ms(1);                           //第一次发送失败, 延时1个滴答, 再次发送
		CAN_Transmit(CANx, &TxMsg);
	//}
}


void CANRcv_DateFromISR(CanRxMsg *RxMsg)
{
	static Message msg;
	uint8_t i = 0;
	msg.cob_id = RxMsg->StdId;                  //CAN-ID

	if(CAN_RTR_REMOTE == RxMsg->RTR)
		msg.rtr = 1;                             //远程帧
	else
		msg.rtr = 0;                             //数据帧

	msg.len = (UNS8)RxMsg->DLC;                 //长度

	for(i=0; i<RxMsg->DLC; i++)                 //数据
		msg.data[i] = RxMsg->Data[i];

	TIM_ITConfig(CANOPEN_TIMx, TIM_IT_Update, DISABLE);

	canDispatch(&Master_Data, &msg);       //调用协议相关接口

	TIM_ITConfig(CANOPEN_TIMx, TIM_IT_Update, ENABLE);
}


/********************************** CANOpen接口函数(需自己实现) **********************************/
/************************************************
函数名称 ： canSend
功    能 ： CAN发送
参    数 ： notused --- 未使用参数
            m --------- 消息参数
返 回 值 ： 0:失败  1:成功
*************************************************/
unsigned char canSend(CAN_PORT notused, Message *m)
{
  uint8_t i;
  static CanTxMsg TxMsg;
	
  TxMsg.StdId = m->cob_id;

  if(m->rtr)
      TxMsg.RTR = CAN_RTR_REMOTE;
  else
      TxMsg.RTR = CAN_RTR_DATA;

  TxMsg.IDE = CAN_ID_STD;
  TxMsg.DLC = m->len;
  for(i=0; i<m->len; i++)
      TxMsg.Data[i] = m->data[i];

  CANSend_Date(TxMsg);

  return 0;
}

/************************************************
函数名称 ： setTimer
功    能 ： Set the timer for the next alarm.
参    数 ： value --- 参数
返 回 值 ： 无
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
*************************************************/
void TIMx_DispatchFromISR(void)
{
  last_counter_val = 0;
  elapsed_time = 0;
  TimeDispatch();
}
