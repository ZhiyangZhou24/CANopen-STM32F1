#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"                  // Device header
#include  "touch_CTP.h"
#include "LCDhard.h" 
#include "delay.h"
#include  "touch.h"
#include "stm32f10x_it.h"
#include "sram.h"	
#include "at24c02.h"

#include "stdio.h"
#include "plot.h"
#include "sdio_sdcard.h"	
#include "GUI.h"
#include "WM.h"
#include "malloc.h"	

#include "bsp_timer.h"
#include "bsp_can.h"
#include "bsp.h"
/*CANopenœ‡πÿ*/
#include "stm32f10x_can.h"  
#include "stm32f10x_usart.h"  
#include "canopen_drv.h"
#include "canopen_app.h"


extern WM_HWIN hWinSetting;
extern WM_HWIN hWinWindow;

#endif
