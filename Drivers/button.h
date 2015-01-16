#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f10x.h"

/* �����˲�ʱ��50ms����λ10ms */
#define BUTTON_FILTER_TIME    13

typedef struct
{
	u8    (*IsBtnFallFunc)(void);		  /* �������µ��жϺ���, 1��ʾ���� */
	
	u8    Count;						  /* �˲��������� */
	u8    FilterTime;					  /* �˲�ʱ��(���255,��ʾ2550ms) */
	u8    BtnCode;					      /* �������µļ�ֵ����, 0��ʾ����ⰴ������ */
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

/* ���API */
void init_button(void);
void put_btn(u8 keycode);
u8   get_btn(void);
void btn_state(void);

#endif /* __BUTTON_H */
