/****************************************************************************
* 文件名: at24cxx.h
* 内容简述: IIC读写AT24C02
*
* 文件历史:
* 版本号  完成日期       作者    说明
* v0.1    2011-8-4      gnlin   创建该文件
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
