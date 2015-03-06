/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00  
* Programmer(s) : Brian Nagel 
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <bsp.h>

/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void BSP_Init (void)
{
	SystemInit();
	/* 初始化SysTick服务 */
	OS_CPU_SysTickInit();
    /* 初始化Usart服务 */
	USART_Configuration();
   	BSP_IntInit();
	BSP_IntVectSet(BSP_INT_ID_RTC, rtc_isr);
	BSP_IntPrioSet(BSP_INT_ID_RTC, RTC_Second_Prio);

	/* 初始化RTC */
	rtc_init();

	/* 初始化按键队列 */
    init_button();
	/* led初始化 */
	BSP_Led_Init();
	/* 初始化OLED显示器 */
	OLED_Init();
	/* 初始化VS1003B */
	vs1003b_init();
	/* 初始化EEPROM访问 */
	AT24CXX_Init();
	/* 初始化SPI Flash访问 */
	W25X16_Init();
}

/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                         OS CORTEX-M3 FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         OS_CPU_SysTickClkFreq()
*
* Description : Get system tick clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : Clock frequency (of system tick).
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

INT32U  OS_CPU_SysTickClkFreq (void)
{
    INT32U  freq;
    freq = BSP_CPU_ClkFreq();
    return (freq);
}

/*
*********************************************************************************************************
*                      BSP_Led()   BSP_Led_Tooggle()   BSP_Led_Init()
*
* Description : control the only led light on board when using temperature sensor.
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_Led(BOOLEAN Light)
{
	if (Light == DEF_ON)
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
	else
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

void BSP_Led_Toggle(void)
{
	if (DEF_ON == GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4))
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	else
		GPIO_SetBits(GPIOC, GPIO_Pin_4);			
}

void BSP_Led_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
