/****************************************************************************
* 文件名: at24cxx.c
* 内容简述: IIC读写AT24C02
*
* 文件历史:
* 版本号  完成日期       作者    说明
* v0.1    2011-8-4      gnlin   创建该文件
*
*/
#include "at24cxx.h"
#include "stm32f10x.h"
#include "delay.h"
#include "iic.h"

void AT24CXX_Init(void)
{
	IIC_Init();
}

// 当前地址读
u8 AT24CXX_CurrentAddrRead(void)
{
	u8 temp = 0;
	IIC_Start();

	IIC_Send_Byte(0xA1);
	IIC_Wait_Ack();
	temp = IIC_Read_Byte(0);
	IIC_Stop();
	return temp;
}

// 指定地址读
u8 AT24CXX_RandomAddrRead(u8 addr)
{
	u8 temp = 0;
	IIC_Start();
	IIC_Send_Byte(0xA0);
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	IIC_Start(); //这里必须重新Start
	IIC_Send_Byte(0xA1);
	IIC_Wait_Ack();
	temp = IIC_Read_Byte(0);
	IIC_Stop();
	return temp;	
}

// 序列读
void AT24CXX_SequentalRead(u8 addr, u8 *buffer, u8 count)
{
	u8 i;
	IIC_Start();
	IIC_Send_Byte(0xA0);
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(0xA1);
	IIC_Wait_Ack();
	for (i = 0; i < count; i++) {
		buffer[i] = IIC_Read_Byte(1);
	}
	IIC_Stop();
}

// 字节写
void AT24CXX_ByteWrite(u8 addr, u8 byte)
{
	IIC_Start();
	IIC_Send_Byte(0xA0);
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	IIC_Send_Byte(byte);
	IIC_Wait_Ack();
	IIC_Stop();
	DelayMS(10);
}

// 页写
void AT24CXX_PageWrite(u8 addr, u8 *buffer, u8 count)
{
	u8 i;
	IIC_Start();
	IIC_Send_Byte(0xA0);
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();

	for (i = 0; i < count; i++) {
		IIC_Send_Byte(buffer[i]);
		IIC_Wait_Ack();
	}
	IIC_Stop();
	DelayMS(10);
}
