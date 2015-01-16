#include "stm32f10x.h"
#include "delay.h"

void DelayUS(u16 time)
{
	u16 i = 0;
	while (time--) {
		i = 10;
		while (i--);
	}
}

void DelayMS(u16 time)
{
	u16 i = 0;
	while (time--) {
		i = 12000;
		while (i--);
	}
}
