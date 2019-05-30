CANOpen\inc目录下19个.h头文件，来自Canfestival->include下目录19个头文件；

CANOpen\inc\stm32目录下3个.h头文件来自Canfestival->include\cm4；
其中canfestival.h文件是函数接口定义（声明），函数内容需要自己实现（位于：App\canopen目录下canopen_drv.c）；


CANOpen\src目录下12个.c源文件，来自Canfestival->src目录12个源文件；
（symbols.c 源文件为 linux 下使用的文件，不需要提取）；
其中需要删除dcf.c文件下第59、98行前面的“inline”关键字；