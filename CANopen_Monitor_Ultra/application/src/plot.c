#include "stm32f10x.h"                  // Device header
#include "plot.h"
#include "math.h"
#include "LCDhard.h" 

void remap(float x,float y, int bis_x, int bis_y, char xsize, char ysize)
{
	static signed int pos_x,pos_y;
	
	DrawLine(bis_x,bis_y,bis_x+400,bis_y,RED);
	DrawLine(bis_x+400,bis_y-200,bis_x+400,bis_y+200,RED);
	DrawLine(bis_x,bis_y+200,bis_x+400,bis_y+200,RED);
	DrawLine(bis_x,bis_y-200,bis_x+400,bis_y-200,RED);
	DrawLine(bis_x,bis_y-200,bis_x,bis_y+200,RED);
	
	pos_x=x*xsize;
	pos_y=y*ysize;
	pos_x=pos_x%400;
		
	DrawPixel(bis_x+pos_x,bis_y+pos_y,RED);
}

void plot_sin()
{
	static float x,y;
	y=cos(x);
	x=x+0.02;
	remap(x,y,400,240,10,100);
	
	
}


