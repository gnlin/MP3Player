/****************************************************************************
* �ļ���: at24cxx.h
* ���ݼ���: IIC��дAT24C02
*
* �ļ���ʷ:
* �汾��  �������       ����    ˵��
* v0.1    2011-8-4      gnlin   �������ļ�
*
*/
#include "stm32f10x.h"

#ifndef AT24CXX_H
#define AT24CXX_H

void AT24CXX_Init(void);
u8 AT24CXX_CurrentAddrRead(void);
u8 AT24CXX_RandomAddrRead(u8 addr);
void AT24CXX_SequentalRead(u8 addr, u8 *buffer, u8 count);
void AT24CXX_ByteWrite(u8 addr, u8 byte);
void AT24CXX_PageWrite(u8 addr, u8 *buffer, u8 count);

#endif
