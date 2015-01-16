#include <includes.h>

u8 Show_RTC = DEF_FALSE;
extern OS_EVENT *AppUserIFMbox;
extern const u8* week[7];

static void display_time_1(u8 x, u8 y)
{
	OLED_DrawNum(x, y, timer.hour, 2, 12);
	OLED_DrawString(x + 12, y, ":", 1);
	OLED_DrawNum(x + 18, y, timer.min, 2, 12);
	OLED_DrawString(x + 30, y, ":", 1);
	OLED_DrawNum(x + 36, y, timer.sec, 2, 12);
	OLED_Refresh_Gram();
}

static void display_time_2(u8 x, u8 y)
{
	OLED_DrawNum(x, y, (u8)(timer.year / 100), 2, 12);
	OLED_DrawNum(x + 12, y, (u8)(timer.year % 100), 2, 12);
	OLED_DrawString(x + 24, y, "-", 1);
	OLED_DrawNum(x + 30, y, timer.month, 2, 12);
	OLED_DrawString(x + 42, y, "-", 1);
	OLED_DrawNum(x + 48, y, timer.day, 2, 12);
	OLED_DrawString(x + 60, y, ",", 1);
 	OLED_DrawNum(x + 66, y, timer.hour, 2, 12);
	OLED_DrawString(x + 78, y, ":", 1);
	OLED_DrawNum(x + 84, y, timer.min, 2, 12);
	OLED_DrawString(x + 96, y, ":", 1);
	OLED_DrawNum(x + 104, y, timer.sec, 2, 12);
	OLED_DrawString(x, y + 12, "Week: ", 1);
	OLED_DrawString(x + 36, y + 12, week[timer.week], 1);
	OLED_Refresh_Gram();
}

void rtc_isr(void)			  
{
	if (RTC->CRL & 0x0001) {
		rtc_get();
		if (Show_RTC == DEF_TRUE)
 			display_time_2(4, 16);
		else
			display_time_1(75, 0);	
	}
	RTC->CRL &= 0x0FFA;
	while (!(RTC->CRL & (1 << 5)));
}

INT8U  Page_Clock_Setting(void)
{
	#define COUNT_MAX   6
	INT8U key_code = BTN_NONE;
	INT8U err;
	INT8U data[COUNT_MAX];
	INT8U count = 0;
	INT8U range = 0;
	INT8U len[6] = {16, 34, 48, 66, 84, 102};

	data[0] = (INT8U)(timer.year % 100);
	data[1] = (INT8U)timer.month;
	data[2] = (INT8U)timer.day;

	data[3] = (INT8U)timer.hour;
	data[4] = (INT8U)timer.min;
	data[5] = (INT8U)timer.sec;

	Show_RTC = DEF_TRUE;
	Page_Draw_Frame(PAGE_CLOCK_SETTING);
	OLED_Fill(0, 15, 127, 63, 0);
	OLED_Refresh_Gram();
		
 	while (1) {
		key_code = BTN_NONE;
		
		OLED_DrawNum(4, 40, 20, 2, 12);
		OLED_DrawNum(4 + 12, 40, data[0], 2, 12);
		OLED_DrawString(4 + 24, 40, "-", 1);
		OLED_DrawNum(4 + 30, 40, data[1], 2, 12);
		OLED_DrawString(4 + 42, 40, "-", 1);
		OLED_DrawNum(4 + 48, 40, data[2], 2, 12);
		OLED_DrawString(4 + 60, 40, ",", 1);
		OLED_DrawNum(4 + 66, 40, data[3], 2, 12);
		OLED_DrawString(4 + 78, 40, ":", 1);
		OLED_DrawNum(4 + 84, 40, data[4], 2, 12);
 		OLED_DrawString(4 + 96, 40, ":", 1);
		OLED_DrawNum(4 + 102, 40, data[5], 2, 12);

		OLED_Fill(0, 52, 127, 52, 0);
		OLED_DrawLine(len[count], 52, 12, Horizontal);
		OLED_Refresh_Gram();

		if (count == 1) 
			range = 12;
		else if (count == 2 && (data[1] % 2) == 1)
			range = 31;
		else if (count == 2 && (data[1] % 2) == 0) {
			if (data[1] == 2)
				range = 28;
			else
				range = 30;
		} else if (count == 3)
			range = 24;
		else if (count == 4 || count == 5)
			range = 60;
		else
			range = 99;

		key_code = *(INT8U *)OSMboxPend(AppUserIFMbox, 0, &err);

		switch (key_code) {
			case BTN_UI_UP:
				data[count]++;
				if (data[count] > range)
					data[count] = 0;	
				break;			
			case BTN_UI_DOWN:
				if (data[count] == 0)
					data[count] = range;
				else
					data[count]--;
				break;
			case BTN_UI_RIGHT:
				count++;
				if (count == COUNT_MAX) count = 0;
				break;
			case BTN_UI_LEFT:
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
				PWR_BackupAccessCmd(ENABLE);
				BKP_DeInit();
				RCC_LSEConfig(RCC_LSE_ON);
				while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
				RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
				RCC_RTCCLKCmd(ENABLE);
				RTC_WaitForSynchro();
				RTC_WaitForLastTask();
				RTC_ITConfig(RTC_IT_SEC, DISABLE);
				RTC_WaitForLastTask();
				rtc_set(2000 + data[0], data[1], data[2], data[3], data[4], data[5]);
				RTC_ITConfig(RTC_IT_SEC, ENABLE);
				RTC_WaitForLastTask();
				rtc_get();
				RCC_ClearFlag();

			case BTN_UI_CONFIRM:
				Show_RTC = DEF_FALSE;
				return PAGE_MAIN_MENU; 		
		}
		OSTimeDlyHMSM(0, 0, 0, 20);
	}				
}
