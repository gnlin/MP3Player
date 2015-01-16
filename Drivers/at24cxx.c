/****************************************************************************
* �ļ���: at24cxx.c
* ���ݼ���: IIC��дAT24C02
*
* �ļ���ʷ:
* �汾��  �������       ����    ˵��
* v0.1    2011-8-4      gnlin   �������ļ�
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

// ��ǰ��ַ��
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

// ָ����ַ��
u8 AT24CXX_RandomAddrRead(u8 addr)
{
	u8 temp = 0;
	IIC_Start();
	IIC_Send_Byte(0xA0);
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	IIC_Start(); //�����������Start
	IIC_Send_Byte(0xA1);
	IIC_Wait_Ack();
	temp = IIC_Read_Byte(0);
	IIC_Stop();
	return temp;	
}

// ���ж�
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

// �ֽ�д
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

// ҳд
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
