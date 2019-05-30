#include "GUI.h"
#include "LCDhard.h" 
#include "touch.h"

void GUI_TOUCH_X_ActivateX(void) 
{
 // XPT2046_WriteCMD(0x90);
}

void GUI_TOUCH_X_ActivateY(void)
{
  //XPT2046_WriteCMD(0xd0);
}

int  GUI_TOUCH_X_MeasureX(void) 
{
	
	 return Get_TOUCH_X_MeasureX(); //·µ»ØXÖá´¥Ãþ×ø±ê
}

int  GUI_TOUCH_X_MeasureY(void) 
{	 
	
		return Get_TOUCH_X_MeasureY();//·µ»ØYÖá´¥Ãþ×ø±ê
}
