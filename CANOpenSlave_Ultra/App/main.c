#include "FreeRTOS.h"
#include "bsp.h"
#include "app.h"

/************************************************
函数名称 ： SysInit
功    能 ： 系统初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： SWUST Tom
*************************************************/
void SysInit(void)
{
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

/************************************************
函数名称 ： main
功    能 ： 主函数入口
参    数 ： 无
返 回 值 ： int
作    者 ： SWUST Tom
*************************************************/
int main(void)
{
  /* 1、SYS初始化 */
  SysInit();

  /* 2、创建任务 */
  AppTaskCreate();

  /* 3、开启任务 */
  vTaskStartScheduler();

  return 0;
}

