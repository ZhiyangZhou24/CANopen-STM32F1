#ifndef __LCD_H
#define __LCD_H
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"


extern unsigned short POINT_COLOR ;
extern unsigned short BACK_COLOR ;

#define   BLACK                0x0000                // o¨²¨¦?¡êo    0,   0,   0 //
#define   RED                 0x001F                // ¨¤?¨¦?¡êo    0,   0, 255 //
#define   GREEN                0x07E0                // ?¨¬¨¦?¡êo    0, 255,   0 //
#define   CYAN                 0x07FF                // ?¨¤¨¦?¡êo    0, 255, 255 //
#define   BLUE                  0xF800                // o¨¬¨¦?¡êo  255,   0,   0 //
#define   MAGENTA              0xF81F                // ?¡¤o¨¬¡êo  255,   0, 255 //
#define   YELLOW               0xFFE0                // ??¨¦?¡êo  255, 255, 0   //
#define   WHITE                0xFFFF                // ¡ã¡Á¨¦?¡êo  255, 255, 255 //
#define   NAVY                 0x000F                // ¨¦?¨¤?¨¦?¡êo  0,   0, 128 //
#define   DGREEN               0x03E0                // ¨¦??¨¬¨¦?¡êo  0, 128,   0 //
#define   DCYAN                0x03EF                // ¨¦??¨¤¨¦?¡êo  0, 128, 128 //
#define   MAROON               0x7800                // ¨¦?o¨¬¨¦?¡êo128,   0,   0 //
#define   PURPLE               0x780F                // ¡Á?¨¦?¡êo  128,   0, 128 //
#define   OLIVE                0x7BE0                // ¨¦?¨¦-?¨¬¡êo128, 128,   0 //
#define   LGRAY                0xC618                // ?¨°¡ã¡Á¨¦?¡êo192, 192, 192 //
#define   DGRAY                0x7BEF                // ¨¦??¨°¨¦?¡êo128, 128, 128 //

//¨®2?t?¨¤1?¦Ì?¡Á¨®o¡¥¨ºy
#define Bank1_LCD_D    (u32)(0x60000080)    //Disp Data ADDR
#define Bank1_LCD_C    (u32)(0x6000007E)	   //Disp Reg ADDR
#define LCD_DATA *(__IO u16 *) (Bank1_LCD_D)
//BL PE0
#define Lcd_Light_ON   GPIOD->BSRR = GPIO_Pin_3;
#define Lcd_Light_OFF  GPIOD->BRR =GPIO_Pin_3;
#define Lcd_Light_toggle (GPIOD->ODR ^= GPIO_Pin_3)

//Lcd3?¨º??¡¥?¡ã??¦Ì¨ª??????o¡¥¨ºy
void Lcd_Configuration(void);
void Lcd_Initialize(void);
void LCD_WR_REG(u16 Index,u16 CongfigTemp);
void WriteComm(u16 CMD);
void WriteData(u16 tem_data);
static void LCD_Rst(void);
//Lcd????????o¡¥¨ºy
void LCD_Clear(int Color);
void Lcd_ColorBox(u16 x,u16 y,u16 xLong,u16 yLong,u16 Color);

void DrawPixel(u16 x, u16 y, int Color);
void DrawLine(u16 x_str,u16 y_str, u16 x_end,u16 y_end, int Color);
void DrawCircle(u16 point_x,u16 point_y,u16 r,int Color);
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r,int Color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);	  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);

u16 GetPoint( u16 x, u16 y);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic);
extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
#endif


