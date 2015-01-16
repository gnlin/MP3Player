#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f10x.h"

/* 按键滤波时间50ms，单位10ms */
#define BUTTON_FILTER_TIME    13

typedef struct
{
	u8    (*IsBtnFallFunc)(void);		  /* 按键按下的判断函数, 1表示按下 */
	
	u8    Count;						  /* 滤波器计数器 */
	u8    FilterTime;					  /* 滤波时间(最大255,表示2550ms) */
	u8    BtnCode;					      /* 按键按下的键值代码, 0表示不检测按键按下 */
} BUTTON_STRUCT;

typedef enum
{
	BTN_NONE = 0,

	BTN_UI_UP,
	BTN_UI_DOWN,
	BTN_UI_LEFT,
	BTN_UI_RIGHT,
	BTN_UI_CONFIRM
} BTN_ENUM;

#define BTN_FIFO_SIZE    20
typedef struct
{
	u8    Buf[BTN_FIFO_SIZE];
	u8    Read;
	u8    Write;
} BTN_FIFO_STRUCT;

/* 相关API */
void init_button(void);
void put_btn(u8 keycode);
u8   get_btn(void);
void btn_state(void);

#endif /* __BUTTON_H */
