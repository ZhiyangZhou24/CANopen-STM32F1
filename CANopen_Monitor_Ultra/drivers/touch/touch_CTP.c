#include "stm32f10x.h"                  // Device header
#ifdef  use_capacitive_touch_panel
#include  "touch_CTP.h"

void GUI_TOUCH_X_ActivateX(void) {}
void GUI_TOUCH_X_ActivateY(void) {}

	volatile u8 keyId = 0;
	
/****************************************************************************************
																	电容屏 I2C 底层驱动

***************************************************************************************/
	
//*************************************************
//函数名称 : void Touch_GPIO_Config(void)  
//功能描述 : 设置触屏的IIC引脚,用软件模拟的方法实现IIC功能
//输入参数 : 
//输出参数 : 
//返回值   : 
//*************************************************
void Touch_GPIO_Config(void) 
{
GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
/*************************IIC**************************************/

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = FT_SCL_Pin; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  FT_SDA_Pin; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FT_INT_Pin;	//INT  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
//	
	//中断引脚连接  这个没有宏定义,
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);

  EXTI_InitStructure.EXTI_Line=EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//使能按键WK_UP所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
	
	I2C1_SCL_OUT(1);
	I2C1_SDA_OUT(1);
	
	I2C1_Stop();
}


void I2C1_Delay_us(u16 cnt)
{
	u16 i;
	for(i=0;i<cnt;i++);
}

void EXTI12_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		EXTI->PR = EXTI_Line12;  				// 清除中断标示
    keyId = 1;
	}
}
	
/************************************************
*	函 数 名: I2C_Start
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
**************************************************/ 
void I2C1_Start(void)  
{ 
	I2C1_SDA_OUT(1);
	I2C1_SCL_OUT(1);
	I2C1_Delay_us(4);
	I2C1_SDA_OUT(0);
	I2C1_Delay_us(4);
	I2C1_SCL_OUT(0); 
} 

/****************************************************
*	函 数 名: I2C_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
****************************************************/
void I2C1_Stop(void)  
{ 
	I2C1_SDA_OUT(0);
	I2C1_SCL_OUT(0);
	I2C1_Delay_us(4);
	I2C1_SDA_OUT(1);
	I2C1_SCL_OUT(1);
	I2C1_Delay_us(4);
}

/************************************** 
*	函 数 名: I2C_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无 
**************************************/
void I2C1_Ack(void) 
{ 
	I2C1_SCL_OUT(0);
	I2C1_SDA_OUT(0);
	I2C1_Delay_us(2);
	I2C1_SCL_OUT(1);
	I2C1_Delay_us(2);
	I2C1_SCL_OUT(0);
} 
/*
*************************************************
*	函 数 名: I2C_NoAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*************************************************
*/
void I2C1_NoAck(void)
{
	I2C1_SCL_OUT(0);
	I2C1_SDA_OUT(1);
	I2C1_Delay_us(2);
	I2C1_SCL_OUT(1);
	I2C1_Delay_us(2);
	I2C1_SCL_OUT(0);	
}
/*************************************************************
*	函 数 名: I2C_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*************************************************************/
uint8_t I2C1_WaitAck(void)
{ 
	__IO uint16_t t = 0;
	I2C1_SDA_OUT(1);  
	I2C1_Delay_us(1);
	I2C1_SCL_OUT(1);
	I2C1_Delay_us(1);
	
	while(I2C1_SDA_IN())
	{
		t++;
		if(t>30)
		{
			I2C1_Stop();
			return 1;
		}	
	}

	I2C1_SCL_OUT(0);
	return 0; 
}

void I2C1_Send_Byte(uint8_t dat)
{
	__IO uint8_t i;

	I2C1_SCL_OUT(0);

	for(i=0; i<8; i++)
	{		
		if(dat & 0x80)
		{
			I2C1_SDA_OUT(1);
		}
		else
		{
			I2C1_SDA_OUT(0);
		}
		I2C1_Delay_us(2);
		I2C1_SCL_OUT(1);	
		I2C1_Delay_us(2);
		I2C1_SCL_OUT(0);
		I2C1_Delay_us(2);
		dat <<= 1;	
	}
}

/************************************************
*	函 数 名: I2C_Read_Byte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*************************************************/
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t I2C1_Read_Byte(uint8_t ack)
{
	
	unsigned char i,receive=0;
	
	I2C1_SDA_OUT(1);
  for(i=0;i<8;i++ )
	{
		I2C1_SCL_OUT(0); 
    I2C1_Delay_us(2);
		I2C1_SCL_OUT(1);
    receive<<=1;
    if(I2C1_SDA_IN())receive++;   
		I2C1_Delay_us(2); 
   }					 
   if (!ack)  I2C1_NoAck();//发送nACK
   else       I2C1_Ack(); //发送ACK   
    
	 return receive;
}




/****************************************************************************************
																	电容屏 FT6336芯片驱动

***************************************************************************************/
typedef struct
{
	uint16_t cx1; //CTP_X1
	uint16_t cy1; //CTP_Y1
	uint16_t cx2; //CTP_X2
	uint16_t cy2; //CTP_Y2
}CTP_Stru;

CTP_Stru	CTP_Dat;

typedef struct 
{
	uint8_t packet_id;
	uint8_t xh_yh;
	uint8_t xl;
	uint8_t yl;
	uint8_t dxh_dyh;
	uint8_t dxl;
	uint8_t dyl;
  uint8_t checksum;
}TpdTouchData;

static uint8_t FT6206_Write_Reg(uint8_t startaddr,uint8_t *pbuf,uint32_t len)
{
	__IO uint16_t i = 0,j = 0;
	
	I2C1_Start();
	//I2C1_Send_Byte(FT6206_ADDR);
	do
	{
		I2C1_Send_Byte(FT6206_ADDR);

		if(++i >= 1000)	//Time Out
		{
			I2C1_Stop();
			return 1;
		}
	}while(I2C1_WaitAck());	//Read Ack
	
	i = 0;
	do
	{
		I2C1_Send_Byte(startaddr);	//Send Sub Address
		if(++i >= 1000)	//Time Out
		{
			I2C1_Stop();
			return 1;
		}
	}while(I2C1_WaitAck());	//Read Ack
	for(i=0; i<len; i++)
	{
		j=0;
		do
		{
			I2C1_Send_Byte(pbuf[i]);
			
			if(++j >= 1000)	//Time Out
			{
				I2C1_Stop();
				return 1;
			}
		}while(I2C1_WaitAck());	//Read Ack
	}
	I2C1_Stop();
	return 0;
}


u8 FT6206_Read_Reg1(u8 RegIndex)
{
	unsigned char receive=0;

	I2C1_Start();
	I2C1_Send_Byte(FT6206_ADDR);
	I2C1_WaitAck();
	I2C1_Send_Byte(RegIndex);
	I2C1_WaitAck();
	
	I2C1_Start();
	I2C1_Send_Byte(FT6206_ADDR+1);
	I2C1_WaitAck();	
	receive=I2C1_Read_Byte(0);
	I2C1_Stop();	 
	return receive;
}

void FT6206_Write_Reg1(u8 RegIndex,u8 data)
{
	I2C1_Start();
	I2C1_Send_Byte(FT6206_ADDR);
	I2C1_WaitAck();
	I2C1_Send_Byte(RegIndex);
	I2C1_WaitAck();
	
	I2C1_Send_Byte(data);
	I2C1_WaitAck();	

	I2C1_Stop();	 
}


void FT6206_Read_RegN(u8 *pbuf,u8 len)
{
	u8 i;
	I2C1_Start();
	I2C1_Send_Byte(FT6206_ADDR+1);
	I2C1_WaitAck();	
	
	for(i=0;i<len;i++)
	{
		if(i==(len-1))  *(pbuf+i)=I2C1_Read_Byte(0);
		else            *(pbuf+i)=I2C1_Read_Byte(1);
	}		
	I2C1_Stop();
}

uint8_t FT6206_Read_Reg(uint8_t *pbuf,uint32_t len)
{
	
	int8_t i=0;

	I2C1_Start();
	I2C1_Send_Byte(FT6206_ADDR);
	I2C1_WaitAck();	
	
	I2C1_Send_Byte(0);
	I2C1_WaitAck();	
  I2C1_Stop();
  
	I2C1_Start();
	I2C1_Send_Byte(FT6206_ADDR+1);
	I2C1_WaitAck();	
	
	for(i=0;i<len;i++)
	{
		if(i==(len-1))  *(pbuf+i)=I2C1_Read_Byte(0);
		else            *(pbuf+i)=I2C1_Read_Byte(1);
	}		
	I2C1_Stop();
  
	return 0;
}


/*******************************************************************************
* Function Name  : FT6206_Read_Reg
* Description    : Read The FT6206
* Parameter      : startaddr: First address
*				   pbuf: The Pointer Point to a buffer
				   len: The length of the read Data 
* Return         : 1:Fail; 0:Success
*******************************************************************************/
uint8_t FT6206_Read_Reg0(uint8_t startaddr,uint8_t *pbuf,uint32_t len)
{
	__IO uint16_t i = 0;
	
	I2C1_Start();
	//I2C1_Send_Byte(FT6206_ADDR);	//Send Slave Address for Write

	do
	{
		I2C1_Send_Byte(FT6206_ADDR);	//Send Slave Address for Write
		if(++i>100)
		{
			I2C1_Stop();
			return 1;
		}
	}while(I2C1_WaitAck());	//Read Ack
///////////////////////////////////////////////////	
	i = 0;		
	do
	{	
		I2C1_Send_Byte(startaddr);	//Send Slave Address for Read
		if(++i > 100)	//Time Out
		{
			I2C1_Stop();
			return 1;
		}
			
	}while(I2C1_WaitAck());//Read Ack
///////////////////////////////////////////////////
	I2C1_Start();
	i = 0;		
	do
	{	
		I2C1_Send_Byte(FT6206_ADDR | 0x01);	//Send Slave Address for Read
			
		if(++i >= 100)	//Time Out
		{
			I2C1_Stop();
			return 1;
		}
			
	}while(I2C1_WaitAck());//Read Ack
	
	//len-1 Data
	for(i=0; i<len-1; i++)
	{
		pbuf[i] = I2C1_Read_Byte(1);	//读取1个字节
		I2C1_Ack();	 //Ack
	}

	pbuf[i] = I2C1_Read_Byte(0);	//Read the last Byte
	I2C1_NoAck(); // NoAck
		
	I2C1_Stop();	 

	return 0;	 
}

static uint8_t CheckSum(uint8_t *buf)
{
	__IO uint8_t i;
	__IO uint16_t sum = 0;

	for(i=0;i<7;i++)
	{
		sum += buf[i];		
	}

	sum &= 0xff;
	sum = 0x0100-sum;
	sum &= 0xff;

	return (sum == buf[7]);
}

/*******************************************************************************
* Function Name  : FT6206_Read_Data
* Description    : Read The FT6206
* Parameter      : startaddr: First address
*				   pbuf: The Pointer Point to a buffer
				   len: The length of the read Data 
* Return         : 1:Fail; 0:Success
*******************************************************************************/

uint8_t CTP_Read(uint8_t flag)
{
	__IO uint16_t DCX = 0,DCY = 0;
	
	TpdTouchData TpdTouchData;

	//memset((uint8_t*)&TpdTouchData,0,sizeof(TpdTouchData));

	//Read the FT6206

	
	if(FT6206_Read_Reg((uint8_t*)&TpdTouchData, sizeof(TpdTouchData)))
	{
//		printf("FT6206 Read Fail!\r\n");
		return 0;
	}
	
	//Check The ID of FT6206
	if(TpdTouchData.packet_id != 0x52)	
	{
//		printf("The ID of FT6206 is False!\r\n");
		return 0;	
	}		
	
	//CheckSum
	if(!CheckSum((uint8_t*)(&TpdTouchData)))
	{
//		printf("Checksum is False!\r\n");
		return 0;
	}
	
	//The Key Of TP		
	if(TpdTouchData.xh_yh == 0xff && TpdTouchData.xl == 0xff
		&& TpdTouchData.yl == 0xff && TpdTouchData.dxh_dyh == 0xff && TpdTouchData.dyl == 0xff)
	{
		/*switch(TpdTouchData.dxl)
		{
			case 0:	return 0;
			case 1: printf("R-KEY\r\n"); break;	 //Right Key
			case 2: printf("M-KEY\r\n"); break;	 //Middle Key
			case 4: printf("L-KEY\r\n"); break;	 //Left Key
			default:;
		}*/		
	}
	else 
	{
		//The First Touch Point
		CTP_Dat.cx1 = (TpdTouchData.xh_yh&0xf0)<<4 | TpdTouchData.xl;
		CTP_Dat.cy1 = (TpdTouchData.xh_yh&0x0f)<<8 | TpdTouchData.yl;

		//The Second Touch Point
		if(TpdTouchData.dxh_dyh != 0 || TpdTouchData.dxl != 0 || TpdTouchData.dyl != 0)
		{	
			DCX = (TpdTouchData.dxh_dyh&0xf0)<<4 | TpdTouchData.dxl;
			DCY = (TpdTouchData.dxh_dyh&0x0f)<<8 | TpdTouchData.dyl;

			DCX <<= 4;
			DCX >>= 4;
			DCY <<= 4;
			DCY >>= 4;

			if(DCX >= 2048)
				DCX -= 4096;
			if(DCY >= 2048)
				DCY -= 4096;

			CTP_Dat.cx2 = CTP_Dat.cx1 + DCX;
			CTP_Dat.cy2 = CTP_Dat.cy1 + DCY;
		}		
	}

	if(CTP_Dat.cx1 == 0 && CTP_Dat.cy1 == 0 && CTP_Dat.cx2 == 0 && CTP_Dat.cy2 == 0)
	{
		return 0;
	}
	
//	if(flag)
//	{	
//		printf("#CP%04d,%04d!%04d,%04d;%04d,%04d\r\n",0,0,CTP_Dat.cx1,CTP_Dat.cy1,CTP_Dat.cx2,CTP_Dat.cy2);
//		memset((uint8_t*)&CTP_Dat, 0, sizeof(CTP_Dat));
//	}
	return 1;
}

u8 a,buf[10];
volatile  u16 touchX=0,touchY=0,lastY=0;

int GUI_TOUCH_X_MeasureX(void)
{
	FT6206_Read_Reg((uint8_t*)&buf, 7);

	if ((buf[2]&0x0f) == 1)
	{
		touchX = (s16)(buf[5] & 0x0F)<<8 | (s16)buf[6];
		touchY = (s16)(buf[3] & 0x0F)<<8 | (s16)buf[4];
		if(touchY==0)
			touchX=0;	
	}
	else
	{
		touchY =0;
		touchX =0;		
	}
	return touchY;
}

int GUI_TOUCH_X_MeasureY(void)
{
	return touchX;
}

void Touch_Test(void)
{
  int16_t x1,y1;

//	GUI_SetColor(GUI_BLUE);
//	GUI_SetFont(&GUI_Font32B_1);
//  GUI_DispStringAt("x =",60,0);
//	GUI_DispStringAt("y =",160,0);
	while(1)
	{
//		FT6206_Write_Reg1(0,0);
//		a = FT6206_Read_Reg1(0);
//		GUI_DispDecAt(a, 0, 50, 4);	
//		
//		a = FT6206_Read_Reg1(0xa3);
//		GUI_DispDecAt(a, 0, 0, 4);
//		a = FT6206_Read_Reg1(0xa6);
//		GUI_DispDecAt(a, 100, 0, 4);
//		a = FT6206_Read_Reg1(0xa8);
//		GUI_DispDecAt(a, 200, 0, 4);

//		a = FT6206_Read_Reg1(0xa7);
//		GUI_DispDecAt(a, 300, 0, 4);	
		
		if(1)
		{
//			keyId = 0;
			FT6206_Read_Reg((uint8_t*)&buf, 7);
	
			if ((buf[2]&0x0f) == 1)
			{
				//读出的数据位480*800
				x1 = (s16)(buf[3] & 0x0F)<<8 | (s16)buf[4];
				y1 = (s16)(buf[5] & 0x0F)<<8 | (s16)buf[6];
			}
			else
			{
				x1 = 0xFFFF;
				y1 = 0xFFFF;
			}
	    if((x1>0)||(y1>0))
			{
				//GUI_DispDecAt(x1, 100, 0, 3);
			//	GUI_DispDecAt(y1, 200, 0, 3);
			}		
		}

	}
}

#endif
