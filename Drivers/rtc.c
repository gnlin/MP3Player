#include "stm32f10x.h"
#include "rtc.h"
#include "delay.h"
#include "usart.h"
#include "bsp.h"

time_handler timer;

//平年的月份日期表
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//月修正数据表	
const u8 table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};

const u8* week[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// BKP->DR1保存是否第一次配置RTC
u8 rtc_init(void)
{
	u8 temp = 0;	

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA6A6)
	{
/*	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		PWR_BackupAccessCmd(ENABLE);
		BKP_DeInit();
		RCC_LSEConfig(RCC_LSE_ON);
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
			temp++;
			DelayMS(10);
		}
		if (temp >= 250) return 1;

		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		BSP_IntEn(BSP_INT_ID_RTC);
		RTC_WaitForLastTask();
		RTC_SetPrescaler(32767);
		RTC_WaitForLastTask();
		rtc_set(2011, 9, 19, 0, 0, 1);
		BKP_WriteBackupRegister(BKP_DR1, 0xABCD);
*/
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		PWR_BackupAccessCmd(ENABLE);
		BKP_DeInit();

		RCC_LSEConfig(RCC_LSE_ON);
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
			temp++;
			DelayMS(10);
		}

		if (temp >= 250) return 1;

		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		BSP_IntEn(BSP_INT_ID_RTC);
		RTC_WaitForLastTask();
		RTC_SetPrescaler(32767);
		RTC_WaitForLastTask();
		rtc_set(2011, 8, 15, 23, 5, 11);
		BKP_WriteBackupRegister(BKP_DR1, 0xA6A6);
	} else {

		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
			printf("\n\rPower On Reset occurred...\n\r");
		}
		if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
			printf("\n\rExternal Resetoccurred...\n\r");
		}
		RTC_WaitForSynchro();
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
//	BSP_IntEn(BSP_INT_ID_RTC);
		BSP_IntEn(BSP_INT_ID_RTC);
		RTC_WaitForLastTask();
	}
	rtc_get();
	RCC_ClearFlag();

	return 0;
}


//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
u8 is_leap_year(u16 year)
{
	if (year % 4 == 0)
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0) 
				return 1;
			else
				return 0;
		} else return 1;
	} else {
		return 0;
	}
}

//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
u8 rtc_get_week(u16 year, u8 month, u8 day)
{
	u16 temp;
	u8 yearH, yearL;

    yearH = year / 100; yearL = year % 100;
	if (yearH > 19) yearL += 100;
	temp = yearL + yearL / 4;
	temp = temp % 7;
	temp = temp + day + table_week[month - 1];
	if (yearL % 4 == 0 && month < 3)
		temp--;
	return (temp % 7);
}

//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表
u8 rtc_set(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
	u16 i;
	u32 seccount = 0;
	if (year < 1970 || year > 2099) return 1;

	for (i = 1970; i < year; i++) {
		if (is_leap_year(i))
			seccount += 31622400;
		else
			seccount += 31536000;
	}
	month--;
	for (i = 0; i < month; i++) {
		seccount += (u32)mon_table[i]*86400;
		if (is_leap_year(year) && i == 1) 
			seccount += 86400;
	}
	seccount += (u32)(day - 1)*86400;
	seccount += (u32)hour*3600;
	seccount += (u32)min*60;
	seccount += sec;

	RTC_WaitForLastTask();
	RTC_SetCounter(seccount);
	RTC_WaitForLastTask();
	return 0;
}

//得到当前的时间	
u8 rtc_get(void)
{
	static u16 daycount = 0;
	u32 timecount = 0;
	u32 temp = 0;
	u16 temp_1 = 0;

	timecount = RTC_GetCounter();
	temp = timecount / 86400;
	if (daycount != temp)
	{
		daycount = temp;
		temp_1 = 1970;
		while (temp >= 365)
		{
			if (is_leap_year(temp_1)) {
				if (temp >= 366)
					temp -= 366;
				 else {
				 	temp_1++;
					break;
				 }
			} else {
				temp -= 365;
			}
			temp_1++;
		}
		timer.year = temp_1;
		temp_1 = 0;
		while (temp >= 28)
		{
			if (is_leap_year(timer.year) && temp_1 == 1)
			{
				if (temp >= 29)
					temp -= 29;
				else
					break;
			} else {
				if (temp >= mon_table[temp_1])
					temp -= mon_table[temp_1];
				else
					break;
			}
			temp_1++;	
		}
		timer.month = temp_1 + 1;
		timer.day = temp + 1;
	}
	temp = timecount % 86400;
	timer.hour = temp / 3600;
	timer.min = (temp % 3600) / 60;
	timer.sec = (temp % 3600) % 60;
	timer.week = rtc_get_week(timer.year, timer.month, timer.day);
	return 0;
}
