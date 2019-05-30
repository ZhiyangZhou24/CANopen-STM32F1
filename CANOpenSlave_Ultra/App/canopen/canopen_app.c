/**
  ******************************************************************************
  * @文件名     ： canopen_app.c
  * @作者       ： SWUST Tom
  * @版本       ： V1.0.0
  * @日期       ： 2019年5月20日
  * @摘要       ： CANOpen应用程序源文件
  ******************************************************************************/
/*----------------------------------------------------------------------------
  更新日志:
  2019年5月20日 V1.0.0:初始版本
  ----------------------------------------------------------------------------*/
/* 包含的头文件 --------------------------------------------------------------*/
#include "canopen_app.h"
#include "canopen_drv.h"
#include "Slave.h"
#include "node_config.h"

#include "bsp.h"



/* 静态申明 ------------------------------------------------------------------*/
static void CANOpen_App_Task(void *pvParameters);


/************************************************
函数名称 ： CANOpen_App_Init
功    能 ： CANOpen应用程序初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void CANOpen_App_Init(void)
{
  BaseType_t xReturn;

  CANOpen_Driver_Init();

  xReturn = xTaskCreate(CANOpen_App_Task, "CANOpen_App_Task", CANOPEN_STACK_SIZE, NULL, CANOPEN_TASK_PRIORITY, NULL);
  if(pdPASS != xReturn)
  {
    return;                                      //创建接收任务失败
  }
}

/************************************************
函数名称 ： CANOpen_App_Task
功    能 ： CANOpen应用任务程序
参    数 ： pvParameters --- 可选参数
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
static void CANOpen_App_Task(void *pvParameters)
{
  unsigned char nodeID = SLAVE_NODE_ID;                   //节点ID

  setNodeId(&Slave_Data, nodeID);
  setState(&Slave_Data, Initialisation);
  setState(&Slave_Data, Operational);

  while(1)
  {
		vTaskDelay(50);
		if(getState(&Slave_Data)!=Operational) RLED_OFF();
		else RLED_TOGGLE();
    Temperture[0]+=0.05;
    /* 应用代码 */
  }
}


