#include "stm32f10x.h"
#include "delay.h"
#include "ds18b20.h"

u8 ds18b20_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS18B20_DQ_CLK, ENABLE);	

	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_PIN;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS18B20_DQ_PORT, &GPIO_InitStructure);

	GPIO_SetBits(DS18B20_DQ_PORT, DS18B20_DQ_PIN);
	ds18b20_reset();
	return ds18b20_check();
}

void ds18b20_reset(void)
{	
	DS18B20_IO_OUT(); 
	Clr_DS18B20_DQ();
	DelayUS(750);
	Set_DS18B20_DQ();
	DelayUS(15);
}

u8 ds18b20_check(void)
{
	u8 retry = 0;
	DS18B20_IO_IN();
	while (DS18B20_DQ_IN() && retry < 200)
	{
		retry++;
		DelayUS(1);
	}
	if(retry >= 200)
		return 1;
	else 
		retry = 0;
	while (!DS18B20_DQ_IN() && retry < 240)
	{
		retry++;
		DelayUS(1);
	};
	if (retry >= 240) return 1;	    
	return 0;
}

s16 ds18b20_get_temp(void)
{
	u8 temp, TL, TH;
	s16 tem;
	ds18b20_start();
	ds18b20_reset();
	ds18b20_check();	 
	ds18b20_write_byte(0xCC);        // skip rom
	ds18b20_write_byte(0xBE);        // convert	    
	TL = ds18b20_read_byte();        // LSB   
	TH = ds18b20_read_byte();        // MSB  
	
	if (TH > 7)
	{
		TH =~ TH;
		TL =~ TL; 
		temp = 0;  
	}
	else 
		temp=1;	  	  
	tem = TH; 
	tem <<= 8;    
	tem += TL;
	tem = (float)tem*0.625; 
	if (temp)
		return tem;
	return -tem;    
}

void ds18b20_start(void)
{
	ds18b20_reset();	   
	ds18b20_check();	 
	ds18b20_write_byte(0xCC);  // skip rom
	ds18b20_write_byte(0x44);  // convert
}

void ds18b20_write_byte(u8 date)
{
	u8 i;
	u8 testb;
	DS18B20_IO_OUT();
	for (i = 1; i <= 8; i++) 
	{
		testb = date & 0x01;
		date = date >> 1;
		if (testb) 
		{
			Clr_DS18B20_DQ();
			DelayUS(2);                            
			Set_DS18B20_DQ();
			DelayUS(60);             
		} else {
			Clr_DS18B20_DQ();
			DelayUS(60);             
			Set_DS18B20_DQ();
			DelayUS(2);                          
		}
	}
}

u8 ds18b20_read_byte(void)
{
	u8 i, j, date = 0;
	for (i = 1; i <= 8; i++) 
	{
		j = ds18b20_read_bit();
		date = (j << 7) | (date >> 1);
	}						    
	return date;
}

u8 ds18b20_read_bit(void)
{
	u8 data;
	DS18B20_IO_OUT();
	Clr_DS18B20_DQ(); 
	DelayUS(2);
	Set_DS18B20_DQ(); 
	DS18B20_IO_IN();
	DelayUS(12);
	if (DS18B20_DQ_IN()) 
		data = 1;
	else
		data = 0;	 
	DelayUS(50);           
	return data;
}
