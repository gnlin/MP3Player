#include <includes.h>

FRESULT result;
FATFS   fs;
DIR     dir_info;
FILINFO file_info;
FIL     file;
UINT    br, bw;
BYTE    buffer[512];

extern OS_EVENT *AppUserIFMbox;

BOOLEAN check_suffix(u8 *str, u8 *suffix, u8 suffixlen)
{
	u8 i, len;
	if (suffix == NULL || (!suffix[0])) return DEF_TRUE;
	len = strlen((void *)str);
	if (len <= suffixlen) return DEF_FALSE;
	for (i = suffixlen; i > 0; i--) {
		if (str[len - i] != suffix[suffixlen - i])
			return DEF_FALSE;
	}
	return DEF_TRUE;
}

static INT8U local_abs(INT8U a, INT8U b)
{
	if (a >= b)
		return a - b;
	else
		return b - a;
}

INT8U Page_Music_List(TCHAR *music_name)
{
	INT8U i = 0;
	INT8U j = 0;
	INT8U count_max;
	static INT8U count = 0;
	INT8U key_code = BTN_NONE;
	INT8U err;

	Page_Draw_Frame(PAGE_MUSIC_LIST);
	
	while (1) {
		i = 0;
		j = 0;

		OLED_Fill(0, 15, 127, 63, 0);

		result = f_opendir(&dir_info, MUSIC_DIR);
		if (result != FR_OK)
			printf("Open Root Directory Error\n\r");
		while (1) {
			
			result = f_readdir(&dir_info, &file_info);

			if (result != FR_OK || file_info.fname[0] == 0) 
			{
				break;
			}

			if (check_suffix((u8 *)file_info.fname, ".MP3", strlen(".MP3")) == DEF_FALSE)
			{	
				continue;
			}
			
			if (local_abs(i, count) <= 3 && j < 4) {
				if (i == count)
					OLED_DrawString(4, 16 + (j * 12), file_info.fname, 0);
				else
					OLED_DrawString(4, 16 + (j * 12), file_info.fname, 1);
				j++;												
			}
			i++;				
		}

		count_max = i - 1;	

		OLED_Refresh_Gram();

		key_code = *(INT8U *)OSMboxPend(AppUserIFMbox, 0, &err);

		switch (key_code) {
			case BTN_UI_UP:
				if (count > 0)
					count--;
				else
					count = count_max;
				break;
			case BTN_UI_DOWN:
				if (count < count_max)
					count++;
				else
					count = 0;
				break;
			case BTN_UI_RIGHT:
				return PAGE_MAIN_MENU;
			case BTN_UI_CONFIRM:
				i = 0;
				f_opendir(&dir_info, MUSIC_DIR);
				while (1) {
					f_readdir(&dir_info, &file_info);
					if (i == count) {
						sprintf(music_name, "%s", file_info.fname);
						break;
					}
					i++;	
				}
				return PAGE_MUSIC_PLAYER; 		
		}
		OSTimeDlyHMSM(0, 0, 0, 20);
	}				
}

INT8U Page_Music_Player(TCHAR *music_name)
{
	INT8U key_code = BTN_NONE;
	INT8U err, i, j, pause = DEF_FALSE;
	INT16U step_show, vol, count = 512;
	TCHAR path[20];

	Page_Draw_Frame(PAGE_MUSIC_PLAYER);
/************************************************************
					Playing...
			*  SONG_NAME
							Vol: |||||
************************************************************/
	OLED_DrawString(40, 20, "Playing...", 1);
	OLED_DrawString(15, 32, music_name, 1);
	OLED_DrawString(4, 48, "Vol: ", 1);
	step_show = 12;
	OLED_Fill(34, 50, 104, 58, 0);
    OLED_Fill(34, 50, 34 + (u8)(70.0 * ((float)step_show / 16.0)), 58, 1);
	
	sprintf(path, "%s", MUSIC_DIR);
	strcat(path, "/");
	strcat(path, music_name); 
	OLED_Refresh_Gram();

	vs1003b_init();
	vs1003b_hard_reset();
	vs1003b_soft_reset();
	vs1003b_setting();
	while (VS_DREQ() == 0);	
    result = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
	br = 1;
	j = 0;
	Set_VS_CS();
	Clr_VS_DCS();
		
	while (1) {
		if (count != 0) {
			result = f_read(&file, buffer, 512, &br);
			count = 0;
		}

		if ((result == FR_OK || br > 0) && pause == DEF_FALSE) {
			 OLED_DrawChar(4, 32, ' ', 12, 1);			 
			 switch (j) {
			 case 0:
			 	OLED_DrawChar(4, 32, '-', 12, 1);
				break;			 	
			 case 1:
			 	OLED_DrawChar(4, 32, '\\', 12, 1);
			 	break;
			 case 2:
			 	OLED_DrawChar(4, 32, '|', 12, 1);
			 	break;
			 case 3:
			 	OLED_DrawChar(4, 32, '/', 12, 1);
			 	break;
			 }
		    OLED_Refresh_Gram();
			if (j == 3)
				j = 0;
			else
				j++;

			while (count < 512)
			{
				if (VS_DREQ() != 0) {
					for (i = 0; i < 32; i++) {
						VS_WriteByte(buffer[count]);
						count++;
					}	
				}
			}
		}	
		if (result || br == 0) break;
		key_code = *(INT8U *)OSMboxPend(AppUserIFMbox, 10, &err);
		switch (key_code) {
			case BTN_UI_LEFT:
				pause = !pause;
				break;
			case BTN_UI_RIGHT:
				Set_VS_DCS();
				f_close(&file);
				return PAGE_MUSIC_LIST; 
			case BTN_UI_UP:
				vol = vs1003b_volume(VOL_UP);
				Clr_VS_DCS();
				step_show += vol;
				OLED_Fill(34, 50, 104, 58, 0);
      			OLED_Fill(34, 50, 34 + (u8)(70.0 * ((float)step_show / 16.0)), 58, 1);
				OLED_Refresh_Gram();
				break;
			case BTN_UI_DOWN: 
				vol = vs1003b_volume(VOL_DOWN);
				Clr_VS_DCS();
				step_show -= vol;
				OLED_Fill(34, 50, 104, 58, 0);
    		    OLED_Fill(34, 50, 34 + (u8)(70.0 * ((float)step_show / 16.0)), 58, 1);
				OLED_Refresh_Gram();
				break;
		}
	}

	count = 2048;
	while (count) {
		if (VS_DREQ() != 0) {
			for (i = 0; i < 32; i++) {
				VS_WriteByte(0x00);
				count--;
			}
		}	
	}
	Set_VS_DCS();
	f_close(&file);
	return PAGE_MUSIC_LIST;
}
