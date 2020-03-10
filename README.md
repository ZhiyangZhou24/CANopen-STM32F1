# 基于STM32F103系列单片机的CANopen主从机PDO&SDO例程

> 这是一个CANopen协议通信例程，硬件平台为STM32F103单片机，实现了简单的PDO和SDO通信，CANopen的实现是移植的开源协议栈CanFestival

## CanFestival
CanFestival（官网链接https://canfestival.org/）, 网上资料很多，入门也很简单，官方的Manul写的很简洁易懂。用CanFestival开源协议栈之前主要是移植协议栈到自己的平台上，支持的平台很多，Linux和类linux，Windows，32位ARM单片机...都可以移植使用，本工程是基于STM32F103平台的应用

![markdown](https://canfestival.org/home_logo.png "canfestival")

## 硬件
使用STM32F103为主控，主机用的STM32F103ZET6，6个从机用的STM32F103C8T6，CAN收发器用的NXP家的TJA1050，便宜、好用。主机要跑emWin就加了块SDRAM。
## 开发环境
Windows平台开发，用的MDK，下载后直接打开project文件夹下的工程文件即可打开工程。CANopen对象字典的编辑器我用的是Can Festival官方推荐的，虽然有点不怎么智能，但是凑活着用吧，毕竟自家的协议栈还得用自家的编辑器不是，避免踩坑嘛（其实官方的编辑器坑也多），另外由于监视用的主机我加了块五寸LCD显示，懒得手写GUI了，直接用的emWin图形软件框架。

### 主、从机
主机用STM32F103ZET6，从机用C8T6，例程里面，由于主机跑emWin，没内存跑操作系统了所以主机就直接裸奔了额，从机跑了FreeRTOS，整个系统就是模拟的工业现场的数据采集功能，从机采集数据通过PDO传输到主机，主机用SDO向从机发送控制命令，主机可以监视所有从机的心跳。
从机代码是通用的，如果需要多个从机，直接复制即可，不过要更改对应的从机ID号。具体更改方法是下载从机代码之后打开工程文件，找到node_coonfig.h文件，更改ID号，也可在这个文件下更改从机的心跳频率。
	#ifndef _NODE_CONFIG_H
	#define _NODE_CONFIG_H
	
	#define SLAVE_NODE_ID           4   //ID为4，可以任意更改，保留0号ID，0号为主机
	#define Producer_Heartbeat_Time 500 //从机心跳频率，任意更改，单位ms
	
	#endif 
## 最后
介绍就是这么多，这个工程是我去年做的了，因为疫情在家不能出门（哭惨），闲的没事，就简单更新下介绍，有什么不懂的可以联系我，
	企鹅号778733609
