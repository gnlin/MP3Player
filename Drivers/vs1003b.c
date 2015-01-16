#include "stm32f10x.h"
#include "vs1003b.h"
#include "delay.h"
#include "spi.h"
	
// initialization
void vs1003b_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	SPI1_Init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	Set_VS_CS();
	Set_VS_DCS();
	Set_VS_RESET();
}

// write command						
void vs1003b_write_command(u8 address, u16 data)
{
	while (VS_DREQ() == 0);
	SPI_SetSpeed(SPI1, SPI_SPEED_64);
	Set_VS_DCS();
	Clr_VS_CS();
	VS_WriteByte(VS_WRITE_COMMAND);
	VS_WriteByte(address);
	VS_WriteByte(data >> 8);
	VS_WriteByte(data);
	Set_VS_CS();
	SPI_SetSpeed(SPI1, SPI_SPEED_8);
}

// read register
u16 vs1003b_read_reg(u8 address)
{
	u16 temp = 0;
	while (VS_DREQ() == 0);
	SPI_SetSpeed(SPI1, SPI_SPEED_64);
    Set_VS_DCS();
	Clr_VS_CS();
	VS_WriteByte(VS_READ_COMMAND);
	VS_WriteByte(address);
	temp = VS_ReadByte();
	temp = temp << 8;
	temp |= VS_ReadByte();
	Set_VS_CS();
	SPI_SetSpeed(SPI1, SPI_SPEED_8);
	return temp;
}

void vs1003b_add_volume(void)
{
}

void vs1003b_reduce_volume(void)
{
}

// write data
void vs1003b_write_data(u8 data)
{
	SPI_SetSpeed(SPI1, SPI_SPEED_8);
	Set_VS_CS();
	Clr_VS_DCS();
	VS_WriteByte(data);
	Set_VS_DCS();
}

// soft reset					
void vs1003b_soft_reset(void)
{
	u8 retry = 0;
	SPI_SetSpeed(SPI1, SPI_SPEED_64);
	while (VS_DREQ() == 0);
	VS_ReadByte();

	while (vs1003b_read_reg(REG_MODE) != 0x0800) // soft reset vs1002 mode
	{
		vs1003b_write_command(REG_MODE, 0x0800);
		DelayMS(2);
		if (retry++ > 100) break;
	}
	while (VS_DREQ() == 0);

	retry = 0;
	while (vs1003b_read_reg(REG_CLOCKF) != 0x9800)
	{
		vs1003b_write_command(REG_CLOCKF, 0x9800);
		if (retry++ > 100) break;
	}

	Clr_VS_DCS();
	VS_ReadByte();
	VS_ReadByte();
	VS_ReadByte();
	VS_ReadByte();
	Set_VS_DCS();
	DelayMS(20);
}
			
// hard reset
u8 vs1003b_hard_reset(void)
{
	u8 retry = 0;
	Clr_VS_RESET();
	DelayMS(20);
	Set_VS_DCS();
	Set_VS_CS();
	Set_VS_RESET();
	while (VS_DREQ() == 0 && retry < 200) {
		retry++;
		DelayMS(1);	
	}
	DelayMS(20);
	if (retry >= 200) return 1;
//	vs1003b_soft_reset();
	return 0;	
}
					
// ram test	ok!
u16 vs1003b_ram_test(void)
{
	vs1003b_hard_reset();
	vs1003b_write_command(REG_MODE, 0x0820);
	while (VS_DREQ() == 0);
	SPI_SetSpeed(SPI1, SPI_SPEED_64);
	Set_VS_CS();
	Clr_VS_DCS();
	VS_WriteByte(0x4D);
	VS_WriteByte(0xEA);
	VS_WriteByte(0x6D);
	VS_WriteByte(0x54);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	DelayMS(50);
	Set_VS_DCS();
	return vs1003b_read_reg(REG_HDAT0);		
}

// sine wave test ok!					
void vs1003b_sine_test(void)
{
	vs1003b_hard_reset();
	vs1003b_write_command(REG_VOL, 0x2020);
	vs1003b_write_command(REG_MODE, 0x0820);
	while (VS_DREQ() == 0);
	SPI_SetSpeed(SPI1, SPI_SPEED_64);
	Set_VS_CS();
	Clr_VS_DCS();
	VS_WriteByte(0x53);
	VS_WriteByte(0xEF);
	VS_WriteByte(0x6E);
	VS_WriteByte(0x24);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	DelayMS(100);
	Set_VS_DCS();

	Clr_VS_DCS();
	VS_WriteByte(0x45);
	VS_WriteByte(0x78);
	VS_WriteByte(0x69);
	VS_WriteByte(0x74);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	DelayMS(100);
	Set_VS_DCS();

	Clr_VS_DCS();
	VS_WriteByte(0x53);
	VS_WriteByte(0xEF);
	VS_WriteByte(0x6E);
	VS_WriteByte(0x44);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	DelayMS(100);
	Set_VS_DCS();

	Clr_VS_DCS();
	VS_WriteByte(0x45);
	VS_WriteByte(0x78);
	VS_WriteByte(0x69);
	VS_WriteByte(0x74);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	VS_WriteByte(0x00);
	DelayMS(100);
	Set_VS_DCS();
}						

void vs1003b_setting(void)
{
	vs1003b_write_command(REG_VOL, 0x3F3F);
}

// control functions
u8 vs1003b_volume(u8 up_or_down)
{
	u16 vol_reg_read;
	u8 i = 0;

	vol_reg_read = vs1003b_read_reg(REG_VOL);
	if (up_or_down == VOL_UP) {
		if (vol_reg_read >= 0x1F1F) {
			vol_reg_read -= VOL_STEP;
			i = 1;
		}
	} else {
		if (vol_reg_read <= 0xEFEF) {
			vol_reg_read += VOL_STEP;
			i = 1;
		}
	}
	vs1003b_write_command(REG_VOL, vol_reg_read);
	return i;
}
