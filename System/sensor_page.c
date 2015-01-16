#include <includes.h>

extern OS_EVENT *AppUserIFMbox;

INT8U  Page_Sensor_Reading(void)
{
	INT8U key_code = BTN_NONE;
	INT8U err;
	OS_CPU_SR  cpu_sr = 0;
	s16 temp = 0;

	Page_Draw_Frame(PAGE_SENSOR);

	while (ds18b20_init()) {
		OLED_DrawString(4, 16, "DS18B20 Check Failed", 1);  
		OLED_Refresh_Gram();
	} 
	OLED_Fill(0, 15, 127, 63, 0);
	while (1) {
		key_code = *(INT8U *)OSMboxPend(AppUserIFMbox, 10, &err);
		
		OS_ENTER_CRITICAL();
		temp = ds18b20_get_temp();
		OS_EXIT_CRITICAL();

		if (temp < 0)
		{
			temp = -temp;
			OLED_DrawChar(40, 40, '-', 16, 1);
		}
		OLED_DrawNum(40 + 8, 40, temp / 10, 2, 16);
		OLED_DrawChar(40 + 24, 40, '.', 16, 1);
		OLED_DrawNum(40 + 32, 40, temp % 10, 1, 16);
		OLED_Refresh_Gram();

		switch (key_code) {
			case BTN_UI_UP:
			case BTN_UI_DOWN:
			case BTN_UI_RIGHT:
			case BTN_UI_LEFT:
				break;
			case BTN_UI_CONFIRM:
				 return PAGE_MAIN_MENU; 		
		}
		OSTimeDlyHMSM(0, 0, 0, 20);
	}				
}
