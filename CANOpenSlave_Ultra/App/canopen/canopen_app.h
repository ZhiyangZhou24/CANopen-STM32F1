/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef _CANOPEN_APP_H
#define _CANOPEN_APP_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "bsp.h"
/* 宏定义 --------------------------------------------------------------------*/
#define CANOPEN_STACK_SIZE        128                      //CANOPEN任务堆栈大小
#define CANOPEN_TASK_PRIORITY     2                        //CANOPEN任务优先级


/* 函数申明 ------------------------------------------------------------------*/
void CANOpen_App_Init(void);


#endif /* _CANOPEN_APP_H */

/**** Copyright (C)2019 SWUST Tom. All Rights Reserved **** END OF FILE ****/
