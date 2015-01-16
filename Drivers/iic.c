/****************************************************************************
* 文件名: iic.c
* 内容简述: 单发送单接收的IIC操作代码，软件实现
*
* 文件历史:
* 版本号  完成日期       作者    说明
* v0.1    2011-8-3      gnlin   创建该文件
* v0.11   2011-8-5      gnlin   原先无法工作，原因是SDA_IN和SDA_OUT的引脚定义出错
*
*/
#include "stm32f10x.h"
#include "iic.h"
#include "delay.h"

// SCL: PB6; SDA: PB7
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IIC_SCL_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IIC_SDA_Pin;
	GPIO_Init(IIC_SDA_Port, &GPIO_InitStructure);

	GPIO_SetBits(IIC_SCL_Port, IIC_SCL_Pin);
	GPIO_SetBits(IIC_SDA_Port, IIC_SDA_Pin);
}

// 开始信号
// SDA
//	1  _______
//			  \
//  0          \_________
// SCL
//	1  __________________
//
//	0
void IIC_Start(void)
{
	SDA_OUT();
	Set_IIC_SDA();
	Set_IIC_SCL();
	DelayMS(4);
	Reset_IIC_SDA();
	DelayMS(4);
	Reset_IIC_SCL();
	DelayMS(4);
}

// 停止信号
// SDA
//	1 			  _______
//			  	 /
//  0  _________/
// SCL
//	1  __________________
//
//	0
void IIC_Stop(void)
{
	SDA_OUT();
	Reset_IIC_SCL();
	Reset_IIC_SDA();
	DelayMS(4);
	Set_IIC_SCL();
	DelayMS(4);
	Set_IIC_SDA();
	DelayMS(4);
}

// 应答信号
// 1: 接收应答失败
// 0：接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ErrTime = 0;

	SDA_IN();
	Set_IIC_SDA();	 // 定义为了输入，设置IO口还会起作用吗？
	Reset_IIC_SCL();
	DelayMS(4);
	Set_IIC_SCL();
	DelayMS(1);

    while (READ_SDA)
	{
		ErrTime++;
		if (ErrTime > 250)
		{
			IIC_Stop();
			return 1;
		}
	}
	Reset_IIC_SCL();
	return 0;	
}

// 产生应答
void IIC_Ack(void)
{
	Reset_IIC_SCL();
	SDA_OUT();
	Reset_IIC_SDA();
	DelayMS(2);
	Set_IIC_SCL();
	DelayMS(2);
	Reset_IIC_SCL();
}

// 发送一个字节
void IIC_Send_Byte(u8 byte)
{
	u8 i;
	SDA_OUT();
	Reset_IIC_SCL();
	for (i = 0; i < 8; i++)
	{
		if ((byte & 0x80) >> 7)
			Set_IIC_SDA();
		else
			Reset_IIC_SDA();
		byte <<= 1;
		DelayMS(2);
		Set_IIC_SCL();
		DelayMS(2);
		Reset_IIC_SCL();
		DelayMS(2);
	}
}

// 读一个字节，ack = 1时发送应答
u8 IIC_Read_Byte(u8 ack)
{
	u8 i, receive = 0;
	SDA_IN();
	for (i = 0; i < 8; i++)
	{
		Reset_IIC_SCL();
		DelayMS(2);
		Set_IIC_SCL();
		receive <<= 1;
		if (READ_SDA) receive++;
		DelayMS(1);
	}
	if (ack) IIC_Ack();
	return receive;	
}
