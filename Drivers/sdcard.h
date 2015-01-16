#ifndef __SDCARD_H
#define __SDCARD_H

#include "stm32f10x.h"

#define NO_RELEASE   0
#define RELEASE      1

// SD card type
#define SD_TYPE_MMC  0
#define SD_TYPE_V1   1
#define SD_TYPE_V2   2
#define SD_TYPE_V2HC 4

// SD card command 	   
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define ACMD23  23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define ACMD41  41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00

//数据写入回应字意义
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

//SD卡回应标记字
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF

#define SD_CS_PORT        GPIOA
#define SD_CS_CLK	  	  RCC_APB2Periph_GPIOA
#define SD_CS_PIN         GPIO_Pin_8
#define Set_SD_CS()       GPIO_SetBits(SD_CS_PORT, SD_CS_PIN)
#define Clr_SD_CS()       GPIO_ResetBits(SD_CS_PORT, SD_CS_PIN)

extern u8 SD_Type;

u8 SD_WaitReady(void);
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc);
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg, u8 crc);
u8 SD_Init(void);
u8 SD_Idle_Sta(void);

u8 SD_ReceiveData(u8 *data, u16 len, u8 release);
u8 SD_GetCID(u8 *cid_data);
u8 SD_GetCSD(u8 *csd_data);
u32 SD_GetCapacity(void);

u8 SD_ReadSingleBlock(u32 sector, u8 *buffer);                  //读一个sector
u8 SD_WriteSingleBlock(u32 sector, const u8 *buffer); 	    	//写一个sector
u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count); 	    //读多个sector
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count);    //写多个sector
u8 SD_Read_Bytes(unsigned long address, u8 *buf, u32 offset, u32 bytes);  //读取byte

#endif /* __SDCARD_H */
