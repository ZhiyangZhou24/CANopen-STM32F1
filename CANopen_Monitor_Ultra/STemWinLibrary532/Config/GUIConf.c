#include "GUI.h"
#include "malloc.h"

//#define USE_EXRAM 0
///设置EMWIN内存大小
//#define GUI_NUMBYTES  200*1024
//#define GUI_BLOCKSIZE 32  //块大小

#define GUI_NUMBYTES  (1024) *  50   // x KByte

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/


/* 32 bit aligned memory area */
U32 extMem[GUI_NUMBYTES / 4];

//GUI_X_Config
//初始化的时候调用,用来设置emwin所使用的内存
void GUI_X_Config(void) {
//	if(USE_EXRAM) //使用外部RAM
//	{	
	//	U32 *aMemory = mymalloc(SRAMEX,GUI_NUMBYTES); //从外部SRAM中分配GUI_NUMBYTES字节的内存
//		GUI_ALLOC_AssignMemory((void*)aMemory, GUI_NUMBYTES); //为存储管理系统分配一个存储块
//		GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //设置存储快的平均尺寸,该区越大,可用的存储快数量越少
//		GUI_SetDefaultFont(GUI_FONT_6X8); //设置默认字体
//	}else  //使用内部RAM
//	{
//		U32 *aMemory = mymalloc(SRAMIN,GUI_NUMBYTES); //从内部RAM中分配GUI_NUMBYTES字节的内存
//		GUI_ALLOC_AssignMemory((U32 *)aMemory, GUI_NUMBYTES); //为存储管理系统分配一个存储块
//		GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //设置存储快的平均尺寸,该区越大,可用的存储快数量越少
		GUI_ALLOC_AssignMemory(extMem, GUI_NUMBYTES);
	
		GUI_SetDefaultFont(GUI_FONT_8X16);
		//GUI_SetDefaultFont(GUI_FONT_6X8); //设置默认字体
	//}
}
