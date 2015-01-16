#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"

typedef struct {
	u8  hour;
	u8  min;
	u8  sec;
	u16 year;
	u8  month;
	u8  day;
	u8  week;
} time_handler;

extern time_handler timer;

extern u8 const mon_table[12];

// operational functions
//void display_time(u8 x, u8 y, u8 size);
//void display_week(u8 x, u8 y, u8 size, u8 lang);
u8 rtc_init(void);
u8 is_leap_year(u16 year);
u8 rtc_get(void);
u8 rtc_get_week(u16 year, u8 month, u8 day);
u8 rtc_set(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec);

#endif /* __RTC_H */
