/****************************************************************************
* 文件名: iic.h
* 内容简述: 单发送单接收的IIC操作代码，软件实现
*
* 文件历史:
* 版本号  完成日期       作者    说明
* v0.1    2011-8-3      gnlin   创建该文件
* v0.11   2011-8-5      gnlin   原先无法工作，原因是SDA_IN和SDA_OUT的引脚定义出错
*
*/

#ifndef IIC_H
#define IIC_H

#include "stm32f10x.h"

#define IIC_SCL_Port   GPIOB
#define IIC_SCL_Pin    GPIO_Pin_6
#define IIC_SDA_Port   GPIOB
#define IIC_SDA_Pin    GPIO_Pin_7

#define SDA_IN()  { IIC_SDA_Port->CRL &= 0x0FFFFFFF; IIC_SDA_Port->CRL |= (4 << 28); }
#define SDA_OUT() { IIC_SDA_Port->CRL &= 0x0FFFFFFF; IIC_SDA_Port->CRL |= (3 << 28); }

#define Set_IIC_SCL()     GPIO_SetBits(IIC_SCL_Port, IIC_SCL_Pin)
#define Reset_IIC_SCL()   GPIO_ResetBits(IIC_SCL_Port, IIC_SCL_Pin)
#define Set_IIC_SDA()     GPIO_SetBits(IIC_SDA_Port, IIC_SDA_Pin)
#define Reset_IIC_SDA()   GPIO_ResetBits(IIC_SDA_Port, IIC_SDA_Pin)

#define READ_SDA          GPIO_ReadInputDataBit(IIC_SDA_Port, IIC_SDA_Pin)

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 byte);
u8 IIC_Read_Byte(u8 ack);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);

#endif
