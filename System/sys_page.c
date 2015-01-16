#include "includes.h"

extern OS_EVENT *AppUserIFMbox;

void Page_Draw_Frame(INT8U Page)
{
	OLED_Fill(0, 0, 127, 63, 0);

	switch (Page) {
		case PAGE_MAIN_MENU:
			OLED_DrawString(4, 0, "Main Menu", 1);
			break;
		case PAGE_MUSIC_LIST:
			OLED_DrawString(4, 0, "Music List", 1);
			break;			
		case PAGE_MUSIC_PLAYER:
			OLED_DrawString(4, 0, "MP3 Player", 1);
			break;
		case PAGE_CLOCK_SETTING:
			OLED_DrawString(4, 0, "Real Time Clock", 1);
			break;	
		case PAGE_SENSOR:
			OLED_DrawString(4, 0, "Temperature", 1);
			break;		
	}
	OLED_DrawLine(0, 14, 127, Horizontal);
}

INT8U  Page_Main_Menu(void)
{
	INT8U flag = 0x01;
   	INT8U key_code = BTN_NONE;
	INT8U err;

	Page_Draw_Frame(PAGE_MAIN_MENU);

	while (1) {
		OLED_DrawString(4, 16, "1.MP3 Player", !(flag & 0x01));
		OLED_DrawString(4, 28, "2.RT Clock", !(flag & 0x02));
		OLED_DrawString(4, 40, "3.Temperature Sensor", !(flag & 0x04));
		OLED_DrawString(4, 52, "4.System Setting", !(flag & 0x08));
		OLED_Refresh_Gram();

		key_code = *(INT8U *)OSMboxPend(AppUserIFMbox, 0, &err);

		switch (key_code) {
			case BTN_UI_UP:
				if (flag == 0x01)
					flag = 0x08;
				else
					flag >>= 1;
				break;
			case BTN_UI_DOWN:
				if (flag == 0x08)
					flag = 0x01;
				else
					flag <<= 1;
				break;
			case BTN_UI_LEFT:
			case BTN_UI_RIGHT:
			case BTN_UI_CONFIRM:
				switch (flag) {
					case 0x01:
						return PAGE_MUSIC_LIST;
					case 0x02:
						return PAGE_CLOCK_SETTING;
					case 0x04:
						return PAGE_SENSOR;
					case 0x08:
					break;
				}
				break;
		}

		OSTimeDlyHMSM(0, 0, 0, 20);
	}
}
