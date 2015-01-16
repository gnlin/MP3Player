#include "stm32f10x.h"
#include "spi.h"
#include "w25x16.h"

#define SPI_Flash_Read()    SPIv_ReadByte()
#define SPI_Flash_Write(x)  SPIv_WriteByte(x)

void W25X16_Write_Enable(void)
{
	Select_Flash();	
	SPI_Flash_Write(WRITE_ENABLE);	
	Deselect_Flash();
}

void W25X16_Write_Disable(void)
{
	Select_Flash();	
	SPI_Flash_Write(WRITE_DISABLE);	
	Deselect_Flash();
}

void W25X16_Read_Id(u8 * id)
{
	u8 i;
	
	Select_Flash();	
	SPI_Flash_Write(READ_ID);	

	for(i=0;i<20;i++)
	{
		id[i] = SPI_Flash_Read();	
	}
	
	Deselect_Flash();
}

u8 W25X16_Read_Status_Reg(void)
{
	u8 sta;
	
	Select_Flash();	
	SPI_Flash_Write(READ_STAUS_REG);	
 
	sta= SPI_Flash_Read();	
	
	Deselect_Flash();
	
	return sta;
}

void W25X16_Write_Status_Reg(u8 reg)
{
	Select_Flash();	
	SPI_Flash_Write(WRITE_STAUS_REG);	
	SPI_Flash_Write(reg);
	Deselect_Flash();
}

//读数据，自动翻页,addr为字节地址
void W25X16_Read_Data(u32 addr, u32 len, u8 *buf)
{
	u32 i;
	
	Select_Flash();	
	SPI_Flash_Write(READ_DATA);	
	SPI_Flash_Write((addr >> 16) & 0xff);
	SPI_Flash_Write((addr >> 8) & 0xff);
	SPI_Flash_Write(addr & 0xff);
	
	for(i=0;i<len;i++)
	{
		buf[i]=SPI_Flash_Read();
	}
	Deselect_Flash();
}

//快速读数据
void W25X16_Fast_Read_Data(u32 addr, u32 len, u8 *buf)
{
	u32 i;

	Select_Flash();	
	SPI_Flash_Write(FAST_READ_DATA);	
	SPI_Flash_Write((addr >> 16) & 0xff);
	SPI_Flash_Write((addr >> 8) & 0xff);
	SPI_Flash_Write(addr & 0xff);
	SPI_Flash_Write(0);
	
	for(i=0;i<len;i++)
	{
		buf[i] = SPI_Flash_Read();
	}
	Deselect_Flash();
}

//页编程函数，页编程前一定要进行页擦除!!!
void W25X16_Page_Program(u32 addr, u16 len, u8 *buf)
{
	u32 i;
	
	W25X16_Write_Enable();
	Select_Flash();	
	SPI_Flash_Write(PAGE_PROGRAM);	
	SPI_Flash_Write((addr >> 16) & 0xff);
	SPI_Flash_Write((addr >> 8) & 0xff);
	SPI_Flash_Write(addr & 0xff);

	for(i=0;i<len;i++)
		SPI_Flash_Write(buf[i]);

	Deselect_Flash();

	while(W25X16_Read_Status_Reg()&0x01);	
}

void W25X16_Sector_Erase(u32 addr)
{
	W25X16_Write_Enable();

	Select_Flash();	
	SPI_Flash_Write(SECTOR_ERASE);	
	SPI_Flash_Write((addr >> 16) & 0xff);
	SPI_Flash_Write((addr >> 8) & 0xff);
	SPI_Flash_Write(addr & 0xff);
	Deselect_Flash();

	while(W25X16_Read_Status_Reg() & 0x01);
}

void W25X16_Bulk_Erase(void)
{
	W25X16_Write_Enable();
	
	Select_Flash();	
	SPI_Flash_Write(BULK_ERASE);	
	Deselect_Flash();

	while(W25X16_Read_Status_Reg() & 0x01);
}

void W25X16_Deep_Power_Down(void)
{
	Select_Flash();	
	SPI_Flash_Write(DEEP_POWER_DOWN);	
	Deselect_Flash();
}

u8 W25X16_Wake_Up(void)
{
	u8 res;

	Select_Flash();	
	SPI_Flash_Write(WAKE_UP);	
	SPI_Flash_Write(0);	
	SPI_Flash_Write(0);	
	SPI_Flash_Write(0);	
	res=SPI_Flash_Read();
	
	Deselect_Flash();
    
	return res;
}

void W25X16_Init(void)
{
	SPIv_Init();
	Deselect_Flash(); 
}
