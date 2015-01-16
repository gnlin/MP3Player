#include "stm32f10x.h"
#include "oled.h"
#include "delay.h"
#include "oled_font.h"

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];

void OLED_Refresh_Gram(void)
{
	u8 i, j;
	for (i = 0; i < 8; i++)
	{
		OLED_Write_Byte(0xB0 + i, OLED_CMD);   // set page 0~7
		OLED_Write_Byte(0x00, OLED_CMD);
		OLED_Write_Byte(0x10, OLED_CMD);
		for (j = 0; j < 128; j++)
			OLED_Write_Byte(OLED_GRAM[j][i], OLED_DATA);
	}	
}

void OLED_Write_Byte(u8 date, u8 cmd)
{
	DATAOUT(date);
	if (cmd)
		Set_Oled_DC();	  // command
	else
		Reset_Oled_DC();  // data

	Select_Oled_CS();
	Reset_Oled_WR();
	Set_Oled_WR();
	Deselect_Oled_CS();
	Set_Oled_DC();		
}

void OLED_Display_On(void)
{
	OLED_Write_Byte(0x8D, OLED_CMD);
	OLED_Write_Byte(0x14, OLED_CMD);
	OLED_Write_Byte(0xAF, OLED_CMD);
}

void OLED_Display_Off(void)
{
	OLED_Write_Byte(0x8D, OLED_CMD);
	OLED_Write_Byte(0x10, OLED_CMD);
	OLED_Write_Byte(0xAE, OLED_CMD);
}

void OLED_Clear(void)
{
	u8 i, j;
	for (i = 0; i < 8; i++)
		for (j = 0; j < 128; j++)
			OLED_GRAM[j][i] = 0x00;
	OLED_Refresh_Gram();
}

void OLED_DrawPoint(u8 x, u8 y, u8 dot)
{
	u8 pos, bx, temp = 0;
	if (x > 127 || y > 63) return;

	pos = 7 - y/8;
	bx = y % 8;

	temp = 1 << (7 - bx);
	if (dot)
		OLED_GRAM[x][pos] |= temp;
	else
		OLED_GRAM[x][pos] &= ~temp;
}

void OLED_DrawLine(u8 x, u8 y, u8 length, u8 mode)
{
	u8 i;

	if (mode == Vertical) {
		for (i = 0; i < length; i++)
			OLED_DrawPoint(x, y + i, 1);
	} else {
	  	for (i = 0; i < length; i++)
			OLED_DrawPoint(x + i, y, 1);
	}
}

void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot)
{
	u8 x, y;
	for (x = x1; x <= x2; x++)
		for (y = y1; y <= y2; y++)
			OLED_DrawPoint(x, y, dot);
	OLED_Refresh_Gram();
}

void OLED_DrawChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
	u8 temp, i, j;
	u8 y0 = y;
	
	chr = chr - ' ';
	
	for (i = 0; i < size; i++) {
		if (size == 12)
			temp = asc2_1206[chr][i];
		else
			temp = asc2_1608[chr][i];

		for (j = 0; j < 8; j++) {
			if (temp & 0x80)
				OLED_DrawPoint(x, y, mode);
			else
				OLED_DrawPoint(x, y, !mode);
			temp <<= 1;
			y++;
			if ((y - y0) == size) {
				y = y0;
				x++;
				break;
			}
		}
	}	
}

static u32 OLED_Pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--) result *= m;
	return result;
}

void OLED_DrawNum(u8 x, u8 y, u32 num, u8 len, u8 size)
{
	u8 i, temp;
	u8 enshow = 0;

	for (i = 0; i < len; i++)
	{
		temp = (num / OLED_Pow(10, len - i - 1)) % 10;
		if (enshow == 0 && i < (len - 1))
		{
			if (temp == 0) {
				OLED_DrawChar(x + (size / 2)*i, y, ' ', size, 1);
				continue;
			} else {
				enshow = 1;
			}
		}
		OLED_DrawChar(x + (size / 2) * i, y, temp + '0', size, 1);
	}
}

void OLED_DrawString(u8 x, u8 y, const u8 *p, u8 mode)
{
	#define MAX_CHAR_POSX 122
	#define MAX_CHAR_POSY 58

	while (*p != '\0') {
		if (x > MAX_CHAR_POSX) {
			x = 0; y += 12;
		}
		if (y > MAX_CHAR_POSY) {
			y = x = 0;
			OLED_Clear();
		}
		OLED_DrawChar(x, y, *p, 12, mode);
		x += 6;
		p++;
	}
}	
	  
void OLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
	                              GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 |
								  GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	Deselect_Oled_CS();
	Set_Oled_DC();
	Set_Oled_RD();
	Set_Oled_WR();

	OLED_Write_Byte(0xAE, OLED_CMD); //关闭显示
	OLED_Write_Byte(0xD5, OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_Write_Byte(80, OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	OLED_Write_Byte(0xA8, OLED_CMD); //设置驱动路数
	OLED_Write_Byte(0X3F, OLED_CMD); //默认0X3F(1/64) 
	OLED_Write_Byte(0xD3, OLED_CMD); //设置显示偏移
	OLED_Write_Byte(0X00, OLED_CMD); //默认为0
	
	OLED_Write_Byte(0x40, OLED_CMD); //设置显示开始行 [5:0],行数.
												    
	OLED_Write_Byte(0x8D, OLED_CMD); //电荷泵设置
	OLED_Write_Byte(0x14, OLED_CMD); //bit2，开启/关闭
	OLED_Write_Byte(0x20, OLED_CMD); //设置内存地址模式
	OLED_Write_Byte(0x02, OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_Write_Byte(0xA1, OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_Write_Byte(0xC0, OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_Write_Byte(0xDA, OLED_CMD); //设置COM硬件引脚配置
	OLED_Write_Byte(0x12, OLED_CMD); //[5:4]配置
	 
	OLED_Write_Byte(0x81, OLED_CMD); //对比度设置
	OLED_Write_Byte(0xEF, OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_Write_Byte(0xD9, OLED_CMD); //设置预充电周期
	OLED_Write_Byte(0xf1, OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_Write_Byte(0xDB, OLED_CMD); //设置VCOMH 电压倍率
	OLED_Write_Byte(0x30, OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	
	OLED_Write_Byte(0xA4, OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_Write_Byte(0xA6, OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_Write_Byte(0xAF, OLED_CMD); //开启显示	 
	OLED_Clear();
}
