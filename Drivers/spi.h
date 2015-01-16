#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

#define SPI_ReadByte(SPIx)  SPI_WriteByte(SPIx, 0xFF)

#define SPI_SPEED_2   0
#define SPI_SPEED_4   1
#define SPI_SPEED_8	  2
#define SPI_SPEED_16  3
#define SPI_SPEED_64  4
#define SPI_SPEED_256 5

u8 SPI_WriteByte(SPI_TypeDef *SPIx, u8 byte);
void SPI_SetSpeed(SPI_TypeDef *SPIx, u8 speedset);

void SPI1_Init(void);
void SPI2_Init(void);

// for SPI Flash
#define SPIv_ReadByte()   SPIv_WriteByte(0xFF)
u8 SPIv_WriteByte(u8 byte);
void SPIv_Init(void);

#endif /* __SPI_H */
