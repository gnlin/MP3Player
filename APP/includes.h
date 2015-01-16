/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           MASTER INCLUDES
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : includes.h
* Version       : V1.10
* Programmer(s) : BAN
*********************************************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>

#include  <ucos_ii.h>

#include  <cpu.h>
#include  <lib_def.h>
//#include  <lib_mem.h>
//#include  <lib_str.h>

#include "stm32f10x.h"

#include  <app_cfg.h>
#include  <bsp.h>

#include "at24cxx.h"
#include "oled.h"
#include "rtc.h"
#include "sdcard.h"
#include "usart.h"
#include "vs1003b.h"
#include "button.h"
#include "ff.h"
#include "delay.h"
#include "w25x16.h"
#include "ds18b20.h"

#include "sys_page.h"
#include "mp3player.h"
#include "rt_clock.h"
#include "sensor_page.h"

#endif
