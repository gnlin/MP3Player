#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h"
#include "oled.h"
#include "usart_printf.h"
#include "iic.h"
#include "at24cxx.h"
#include "rtc.h"
#include "ff.h"
#include "vs1003b.h"
#include "mp3player.h"

void InitBoard(void);

int main(void)
{
	u8 i;
	u16 reg_value;

	InitBoard();

	printf("STM32 RTC Check Program\n\r");
/**************************** IIC Test *******************************/
/*	printf("Begin IIC Test Program...\n\r");

	AT24CXX_PageWrite(0, buffer, 8);	
	printf("Start Reading....\n\r");
	AT24CXX_SequentalRead(0, buffer_1, 8);
	for (i = 0; i < 8; i++) {
		printf("%x ", buffer_1[i]);
	}
	printf("\n\r");
*/ 
/**************************** IIC Test *******************************/
/*
 	vs1003b_init();
	printf("vs1003b_ram_test\n\r");
	reg_value = vs1003b_ram_test();
	printf("0x807F is desired, reg_value = 0x%x\n\r", reg_value);
	printf("vs1003b_sine_test\n\r");
	vs1003b_sine_test();
	printf("Do you hear something?\n\r");
*/
	printf("play music\n\r");
	play_music();
	i = 0;

	while(1) {

/*********************************************************/
		if (i == 0) {
			GPIO_SetBits(GPIOC, GPIO_Pin_4);
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);
			i = 1;
		} else {
			GPIO_ResetBits(GPIOC, GPIO_Pin_4);
			GPIO_SetBits(GPIOB, GPIO_Pin_12);
			i = 0;
		}
/*********************************************************/
		DelayMS(500);
	}
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		       
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	    
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		       
	NVIC_Init(&NVIC_InitStructure);		 
}

static void InitBoard(void)
{
	SystemInit();
	SysTick_Config(SystemFrequency / 1000);

	USART_Configuration();
	GPIO_Configuration();
	NVIC_Configuration();
	OLED_Init();
	rtc_init();
	AT24CXX_Init();
}


