#ifndef __VS1003B_H
#define __VS1003B_H

#include "stm32f10x.h"
#include "spi.h"

#define VOL_STEP     0x1010
#define VOL_UP       0x00
#define VOL_DOWN     0x01

#define VS1003B_PORT    GPIOA
#define VS_CS_PIN       GPIO_Pin_1
#define VS_DCS_PIN      GPIO_Pin_2
#define VS_DREQ_PIN     GPIO_Pin_3
#define VS_RESET_PIN    GPIO_Pin_4

#define Set_VS_CS()       GPIO_SetBits(VS1003B_PORT, VS_CS_PIN)
#define Clr_VS_CS()       GPIO_ResetBits(VS1003B_PORT, VS_CS_PIN)
#define Set_VS_DCS()      GPIO_SetBits(VS1003B_PORT, VS_DCS_PIN)
#define Clr_VS_DCS()      GPIO_ResetBits(VS1003B_PORT, VS_DCS_PIN)
#define Set_VS_RESET()    GPIO_SetBits(VS1003B_PORT, VS_RESET_PIN)
#define Clr_VS_RESET()    GPIO_ResetBits(VS1003B_PORT, VS_RESET_PIN)

#define VS_DREQ()         GPIO_ReadInputDataBit(VS1003B_PORT, VS_DREQ_PIN)

#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03		 		 

//寄存器定义
#define REG_MODE        	0x00   //模式控制
#define REG_STATUS      	0x01   //状态寄存器
#define REG_BASS        	0x02   //重音高音设置
#define REG_CLOCKF      	0x03   //时钟设置
#define REG_DECODE_TIME 	0x04   // 1秒之内解码次数
#define REG_AUDATA      	0x05   // 15-1bit采样率，0bit通道数
#define REG_WRAM        	0x06   //上载程序和指令到ram
#define REG_WRAMADDR    	0x07   //在REG_WRAM之前使用
#define REG_HDAT0       	0x08   //文件信息
#define REG_HDAT1       	0x09   //文件信息
#define REG_AIADDR      	0x0a   //指定应用代码开始地址
#define REG_VOL         	0x0b   //音量控制
#define REG_AICTRL0     	0x0c   //
#define REG_AICTRL1     	0x0d   //
#define REG_AICTRL2     	0x0e   //
#define REG_AICTRL3     	0x0f   //

//Mode寄存器位定义
#define SM_DIFF         	0x01   //0正常相位，1左声道反转
#define SM_JUMP         	0x02   //无意义
#define SM_RESET        	0x04   //置位时复位，复位后自动清零
#define SM_OUTOFWAV     	0x08   //置位时停止wav解码
#define SM_PDOWN        	0x10   // 置位时进入软件省电模式
#define SM_TESTS        	0x20   //置位时进入SDI测试模式
#define SM_STREAM       	0x40   //使能vs103的流模式
#define SM_PLUSV        	0x80   //MP3+V解码使能
#define SM_DACT         	0x100   // 控制sck上升沿或下降沿有效
#define SM_SDIORD       	0x200   //控制sdi总线字节顺序
#define SM_SDISHARE     	0x400   //置位时，xDCS可省去，只需xCS
#define SM_SDINEW       	0x800   //置位进入新模式
#define SM_ADPCM        	0x1000   //置位时ADPCM录音使能
#define SM_ADPCM_HP     	0x2000 //置位时使能ADPCM高通滤波器
#define SM_LINE_IN	        0x4000		// 1选择line in，0选择麦克风

#define VS_WriteByte(x)     SPI_WriteByte(SPI1, x)
#define VS_ReadByte()       SPI_ReadByte(SPI1)

// operation functions
u16 vs1003b_read_reg(u8 address);                   // read register
void vs1003b_write_data(u8 data);					// write data
void vs1003b_write_command(u8 address, u16 data);	// write command
void vs1003b_init(void);							// initialization
u8 vs1003b_hard_reset(void);						// hard reset
void vs1003b_soft_reset(void);						// soft reset
u16 vs1003b_ram_test(void);						    // ram test
void vs1003b_sine_test(void);						// sine wave test
void vs1003b_add_volume(void);
void vs1003b_reduce_volume(void);
void vs1003b_setting(void);                         // set vs1003b ready for mp3player
u8 vs1003b_volume(u8 up_or_down);

#endif /* __VS1003B_H */
