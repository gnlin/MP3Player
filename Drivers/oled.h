#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

#define OLED_CS_PIN  GPIO_Pin_0
#define OLED_DC_PIN  GPIO_Pin_1
#define OLED_WR_PIN  GPIO_Pin_2
#define OLED_RD_PIN  GPIO_Pin_3

#define Select_Oled_CS()    GPIO_ResetBits(GPIOC, OLED_CS_PIN)
#define Deselect_Oled_CS()  GPIO_SetBits(GPIOC, OLED_CS_PIN)
#define Set_Oled_DC()    GPIO_SetBits(GPIOC, OLED_DC_PIN)
#define Reset_Oled_DC()  GPIO_ResetBits(GPIOC, OLED_DC_PIN)
#define Set_Oled_WR()    GPIO_SetBits(GPIOC, OLED_WR_PIN)
#define Reset_Oled_WR()  GPIO_ResetBits(GPIOC, OLED_WR_PIN)
#define Set_Oled_RD()    GPIO_SetBits(GPIOC, OLED_RD_PIN)
#define Reset_Oled_RD()  GPIO_ResetBits(GPIOC, OLED_RD_PIN)

#define OLED_CMD   0
#define OLED_DATA  1

#define Horizontal	 0
#define Vertical     1

#define DATAOUT(x)  GPIOC->ODR = (GPIOC->ODR & 0xE01F) | ((x << 5) & 0x1FE0)

// base functions
void OLED_Write_Byte(u8 date, u8 cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);

void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x, u8 y, u8 dot);
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot);
void OLED_DrawChar(u8 x, u8 y, u8 chr, u8 size, u8 mode);
void OLED_DrawNum(u8 x, u8 y, u32 num, u8 len, u8 size);
void OLED_DrawString(u8 x, u8 y, const u8 *p, u8 mode);
void OLED_DrawLine(u8 x, u8 y, u8 length, u8 mode);

#endif /* __OLED_H */
