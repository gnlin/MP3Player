#include "stm32f10x.h"
#include "button.h"

static BUTTON_STRUCT    BtnUp;
static BUTTON_STRUCT    BtnDown;
static BUTTON_STRUCT    BtnLeft;
static BUTTON_STRUCT    BtnRight;
static BUTTON_STRUCT    BtnConfirm;

static BTN_FIFO_STRUCT  BtnQueue;

static u8 IsBtnUpFall(void)       {if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)  == Bit_RESET) return 1; return 0;}
static u8 IsBtnDownFall(void)     {if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)  == Bit_RESET) return 1; return 0;}
static u8 IsBtnLeftFall(void)     {if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_RESET) return 1; return 0;}
static u8 IsBtnRightFall(void)    {if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)  == Bit_RESET) return 1; return 0;} 
static u8 IsBtnConfirmFall(void)  {if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)  == Bit_RESET) return 1; return 0;}  

void init_button(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
	                       RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    BtnQueue.Read = 0;
	BtnQueue.Write = 0;

	BtnUp.IsBtnFallFunc = IsBtnUpFall;
	BtnUp.Count = 0;
	BtnUp.FilterTime = BUTTON_FILTER_TIME;
	BtnUp.BtnCode = BTN_UI_UP;

	BtnDown.IsBtnFallFunc = IsBtnDownFall;
	BtnDown.Count = 0;
	BtnDown.FilterTime = BUTTON_FILTER_TIME;
	BtnDown.BtnCode = BTN_UI_DOWN;

	BtnLeft.IsBtnFallFunc = IsBtnLeftFall;
	BtnLeft.Count = 0;
	BtnLeft.FilterTime = BUTTON_FILTER_TIME;
	BtnLeft.BtnCode = BTN_UI_LEFT;

	BtnRight.IsBtnFallFunc = IsBtnRightFall;
	BtnRight.Count = 0;
	BtnRight.FilterTime = BUTTON_FILTER_TIME;
	BtnRight.BtnCode = BTN_UI_RIGHT;

	BtnConfirm.IsBtnFallFunc = IsBtnConfirmFall;
	BtnConfirm.Count = 0;
	BtnConfirm.FilterTime = BUTTON_FILTER_TIME;
	BtnConfirm.BtnCode = BTN_UI_CONFIRM;
}

void put_btn(u8 keycode)
{
	BtnQueue.Buf[BtnQueue.Write] = keycode;
	
	if (++BtnQueue.Write >= BTN_FIFO_SIZE)
	{
		BtnQueue.Write = 0;
	}	
}

u8 get_btn(void)
{
	u8 ret;

	if (BtnQueue.Read == BtnQueue.Write)
	{
		return BTN_NONE;
	} else {
		ret = BtnQueue.Buf[BtnQueue.Read];

		if (++BtnQueue.Read >= BTN_FIFO_SIZE)
		{
			BtnQueue.Read = 0;
		}
		return ret;
	}
}

static void detect_btn(BUTTON_STRUCT *pBtn)
{
	if (pBtn->IsBtnFallFunc()) {
		if (pBtn->Count < pBtn->FilterTime)
			pBtn->Count++;
		else {
			if (pBtn->BtnCode > 0)
				put_btn(pBtn->BtnCode);
			pBtn->Count = 0;
		}
	}
}

void btn_state(void)
{
	detect_btn(&BtnUp);
	detect_btn(&BtnDown);
	detect_btn(&BtnLeft);
	detect_btn(&BtnRight);
	detect_btn(&BtnConfirm);
}
