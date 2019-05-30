#include "app.h"
#include "bsp.h"
#include "bsp_usart.h"
#include "canopen_app.h"


/* 静态申明 ------------------------------------------------------------------*/
static void APP_Init(void);
static void APP_Task(void *pvParameters);


/************************************************
函数名称 ： AppTaskCreate
功    能 ： 创建任务
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void AppTaskCreate(void)
{
  BaseType_t xReturn;

  /* 创建应用程序(开始任务) */
  xReturn = xTaskCreate(APP_Task, "APP_Task", APP_STACK_SIZE, NULL, APP_TASK_PRIORITY, NULL);
  if(pdPASS != xReturn)
  {
    printf("APP_Task create failed");
    return;                                      //创建任务失败
  }
}

/************************************************
函数名称 ： APP_Init
功    能 ： 应用程序初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
static void APP_Init(void)
{
  CANOpen_App_Init();
}

/************************************************
函数名称 ： APP_Task
功    能 ： 应用程序(开始任务)
参    数 ： pvParameters --- 可选参数
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
static void APP_Task(void *pvParameters)
{
  BSP_Init();                                    //底层驱动初始化
  APP_Init();                                    //应用程序初始化

  for(;;)
  {
    vTaskDelay(50);
		
    BLED_TOGGLE();                                //LED闪烁
  }
}
