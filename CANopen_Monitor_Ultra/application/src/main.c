#include "main.h"

extern volatile GUI_TIMER_TIME OS_TimeMS;

int GUIFlag=0;
int CANopenFlag=0;

WM_HWIN hWinSetting;
WM_HWIN hWinWindow;

extern WM_HWIN Createsetting(void);
extern WM_HWIN CreateWindow(void);

extern void WinMain_DateUpdate(void);

SD_Error err;
int main()
{
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//四位抢占优先级，0位子优先级
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/*系统外设初始化*/
	BSP_Init();
	CANOpen_Driver_Init();
	/*emWin初始化*/
	GUI_Init();
	WM_SetCreateFlags( WM_CF_MEMDEV);/*to use mem dev*/
	
	/*窗口初始化*/
	//hWinSetting = Createsetting();
	hWinWindow = CreateWindow();
	
	while(1)
	{
		if(OS_TimeMS-GUIFlag>50)//20HZ
		{
			GUIFlag=OS_TimeMS;
			GUI_TOUCH_Exec();	
			GUI_Delay(1);
		}
		
		if(OS_TimeMS-CANopenFlag>50)//50HZ
		{
			CANopenFlag = OS_TimeMS;
			CANOpen_App_Task();
			WinMain_DateUpdate();
		}
	}
}
