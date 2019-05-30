#include "stm32f10x.h"                  // Device header
#include "i2c.h"
#include "LCDhard.h" 
#include "at24c02.h"
#define WRITE_ADDR (unsigned char)0xa0
#define READ_ADDR (unsigned char)	0xa1	
void At24c02_init()
{
	i2c_init();
}

void ByteWrite(unsigned char addr,unsigned char data)
{
	I2CStart();
	I2CSendByte(WRITE_ADDR);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CSendByte(data);
	I2CWaitAck();
	I2CStop();
	delay1(100000);
}

unsigned char ReadByte(unsigned char addr)
{
	unsigned char data;
	I2CStart();
	I2CSendByte(WRITE_ADDR);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(READ_ADDR);
	I2CWaitAck();
	data = I2CReceiveByte();
	I2CSendNotAck();
	I2CStop();
	delay1(100000);
	return data;
}


void test()
{
	unsigned char data = ReadByte(0x00);
	
	if(data==100)
	{
		LCD_ShowxNum(400,190,data,6,16,0);
	}
	else
	{
		ByteWrite(0x00,100);
		LCD_ShowString( 400,190,10,16,16,"-");
	}
}
