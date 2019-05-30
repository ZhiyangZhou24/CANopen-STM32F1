#ifndef _CANOPEN_APP_H
#define _CANOPEN_APP_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f10x.h"
#include "bsp_can.h"
/* 宏定义 --------------------------------------------------------------------*/
#define CANOPEN_STACK_SIZE        128                      //CANOPEN任务堆栈大小
#define CANOPEN_TASK_PRIORITY     2                        //CANOPEN任务优先级


/* 函数申明 ------------------------------------------------------------------*/
void CANOpen_Node_Init(void);
void CANOpen_App_Task(void);

#endif /* _CANOPEN_APP_H */
