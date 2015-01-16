#ifndef __W25X16_H
#define __W25X16_H

#include "stm32f10x.h"

// SPI 50MHz最高
//页大小和页数目
#define W25X16_Page_Size 256
#define W25X16_Page_Max 8192

//功能寄存器地址
// 必须在每个Page Program, Sector Erase, 
// Bulk Erase, Write Status Register 指令之前写
#define WRITE_ENABLE 0x06
#define WRITE_DISABLE 0X04
#define READ_ID 0X9F
#define READ_STAUS_REG 0X05
#define WRITE_STAUS_REG 0X01

#define READ_DATA 0X03
#define FAST_READ_DATA 0X0B
#define PAGE_PROGRAM 0X02
#define SECTOR_ERASE 0XD8
#define BULK_ERASE 0XC7
#define DEEP_POWER_DOWN 0XB9
#define WAKE_UP 0XAB

//片选
#define Select_Flash()     GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#define Deselect_Flash()   GPIO_SetBits(GPIOB, GPIO_Pin_8)

void W25X16_Write_Enable(void);
void W25X16_Write_Disable(void);
void W25X16_Read_Id(u8 *id);
u8 W25X16_Read_Status_Reg(void);
void W25X16_Write_Status_Reg(u8 reg);

void W25X16_Read_Data(u32 addr, u32 len, u8 *buf);
void W25X16_Fast_Read_Data(u32 addr, u32 len, u8 *buf);

void W25X16_Page_Program(u32 addr, u16 len, u8 *buf);
void W25X16_Sector_Erase(u32 addr);
void W25X16_Bulk_Erase(void);
void W25X16_Deep_Power_Down(void);
u8 W25X16_Wake_Up(void);
void W25X16_Init(void);

#endif /* __W25X16_H */
