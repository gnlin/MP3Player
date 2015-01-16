#include "stm32f10x.h"
#include "spi.h"

u8 SPI_WriteByte(SPI_TypeDef *SPIx, u8 Byte)
{
	while ((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET);
	SPIx->DR = Byte;
	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == RESET);
	return SPIx->DR;
}

//SPI 速度设置函数
//SpeedSet:
//SPI_SPEED_2   2分频   (SPI 36M@sys 72M)
//SPI_SPEED_8   8分频   (SPI 9M@sys 72M)
//SPI_SPEED_16  16分频  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256分频 (SPI 281.25K@sys 72M)
void SPI_SetSpeed(SPI_TypeDef *SPIx, u8 speedset)
{
	SPIx->CR1 &= 0xFFC7;
	if (speedset == SPI_SPEED_2) {
		SPIx->CR1 |= SPI_BaudRatePrescaler_2;
	} else if (speedset == SPI_SPEED_4) {
		SPIx->CR1 |= SPI_BaudRatePrescaler_4;
	} else if (speedset == SPI_SPEED_8) {
		SPIx->CR1 |= SPI_BaudRatePrescaler_8;
	} else if (speedset == SPI_SPEED_16) {
		SPIx->CR1 |= SPI_BaudRatePrescaler_16;
	} else if (speedset == SPI_SPEED_64) {
		SPIx->CR1 |= SPI_BaudRatePrescaler_64;
	} else {
		SPIx->CR1 |= SPI_BaudRatePrescaler_256;
	}
	SPIx->CR1 |= 1<<6;	
}

void SPI1_Init(void) //for vs1003b
{
  	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 ,ENABLE);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_Cmd(SPI1, ENABLE);  
}

void SPI2_Init(void) // for sd card	
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 ,ENABLE);
	   
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_Cmd(SPI2, ENABLE);   
}

void SPIv_Init(void) // for spi flash
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

#define SPIv_SetData(d) { if (d & 0x80) GPIO_SetBits(GPIOB, GPIO_Pin_10); else GPIO_ResetBits(GPIOB, GPIO_Pin_10); }
#define SPIv_ReadData() GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)

#define SPIv_SetClk()    GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define SPIv_ResetClk()  GPIO_ResetBits(GPIOB, GPIO_Pin_11)

u8 SPIv_WriteByte(u8 byte)
{
	u8 i, read;
	
	for (i = 8; i > 0; i--) {
		SPIv_ResetClk();
		SPIv_SetData(byte);
		byte <<= 1;
		SPIv_SetClk();
		read <<= 1;
		read |= SPIv_ReadData();
	}
	SPIv_SetClk();
	return read;
}
