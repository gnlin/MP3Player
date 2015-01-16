#ifndef __DS18B20_H
#define __DS18B20_H
 
#include "stm32f10x.h"  

#define DS18B20_IO_IN()  {GPIOB->CRH &= 0xFFF0FFFF; GPIOB->CRH |= 8 << 16;}		
#define DS18B20_IO_OUT() {GPIOB->CRH &= 0xFFF0FFFF; GPIOB->CRH |= 3 << 16;}	

#define DS18B20_DQ_PORT              GPIOB
#define DS18B20_DQ_CLK               RCC_APB2Periph_GPIOB  
#define DS18B20_DQ_PIN               GPIO_Pin_12
#define Set_DS18B20_DQ()             GPIO_SetBits(DS18B20_DQ_PORT, DS18B20_DQ_PIN);
#define Clr_DS18B20_DQ()             GPIO_ResetBits(DS18B20_DQ_PORT, DS18B20_DQ_PIN);

#define DS18B20_DQ_IN()              GPIO_ReadInputDataBit(DS18B20_DQ_PORT, DS18B20_DQ_PIN)
  
u8 ds18b20_init(void);
s16 ds18b20_get_temp(void);
void ds18b20_start(void);
void ds18b20_write_byte(u8 date);
u8 ds18b20_read_byte(void);
u8 ds18b20_read_bit(void);
u8 ds18b20_check(void);
void ds18b20_reset(void);

#endif /* __DS18B20_H */
