#ifndef __SYS_PAGE_H
#define __SYS_PAGE_H

#define PAGE_MAIN_MENU     0
#define PAGE_MUSIC_LIST    1
#define PAGE_MUSIC_PLAYER  2
#define PAGE_CLOCK_SETTING 3
#define PAGE_SENSOR        4
#define PAGE_SYS_SETTING   5

void Page_Draw_Frame(INT8U Page);
INT8U  Page_Main_Menu(void);

#endif /* __SYS_PAGE_H */
