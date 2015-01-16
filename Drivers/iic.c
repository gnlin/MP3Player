/****************************************************************************
* �ļ���: iic.c
* ���ݼ���: �����͵����յ�IIC�������룬���ʵ��
*
* �ļ���ʷ:
* �汾��  �������       ����    ˵��
* v0.1    2011-8-3      gnlin   �������ļ�
* v0.11   2011-8-5      gnlin   ԭ���޷�������ԭ����SDA_IN��SDA_OUT�����Ŷ������
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

// ��ʼ�ź�
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

// ֹͣ�ź�
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

// Ӧ���ź�
// 1: ����Ӧ��ʧ��
// 0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ErrTime = 0;

	SDA_IN();
	Set_IIC_SDA();	 // ����Ϊ�����룬����IO�ڻ�����������
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

// ����Ӧ��
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

// ����һ���ֽ�
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

// ��һ���ֽڣ�ack = 1ʱ����Ӧ��
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
