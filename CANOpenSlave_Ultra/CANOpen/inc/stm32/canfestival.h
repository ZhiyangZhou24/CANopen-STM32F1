#ifndef _CANFESTIVAL_H
#define _CANFESTIVAL_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "applicfg.h"
#include "data.h"


/* 函数申明 ------------------------------------------------------------------*/
void initTimer(void);
void clearTimer(void);

unsigned char canSend(CAN_PORT notused, Message *m);
unsigned char canInit(CO_Data * d, uint32_t bitrate);
void canClose(void);

void disable_it(void);
void enable_it(void);


#endif /* _CANFESTIVAL_H */
