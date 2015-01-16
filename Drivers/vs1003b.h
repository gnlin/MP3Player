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

//�Ĵ�������
#define REG_MODE        	0x00   //ģʽ����
#define REG_STATUS      	0x01   //״̬�Ĵ���
#define REG_BASS        	0x02   //������������
#define REG_CLOCKF      	0x03   //ʱ������
#define REG_DECODE_TIME 	0x04   // 1��֮�ڽ������
#define REG_AUDATA      	0x05   // 15-1bit�����ʣ�0bitͨ����
#define REG_WRAM        	0x06   //���س����ָ�ram
#define REG_WRAMADDR    	0x07   //��REG_WRAM֮ǰʹ��
#define REG_HDAT0       	0x08   //�ļ���Ϣ
#define REG_HDAT1       	0x09   //�ļ���Ϣ
#define REG_AIADDR      	0x0a   //ָ��Ӧ�ô��뿪ʼ��ַ
#define REG_VOL         	0x0b   //��������
#define REG_AICTRL0     	0x0c   //
#define REG_AICTRL1     	0x0d   //
#define REG_AICTRL2     	0x0e   //
#define REG_AICTRL3     	0x0f   //

//Mode�Ĵ���λ����
#define SM_DIFF         	0x01   //0������λ��1��������ת
#define SM_JUMP         	0x02   //������
#define SM_RESET        	0x04   //��λʱ��λ����λ���Զ�����
#define SM_OUTOFWAV     	0x08   //��λʱֹͣwav����
#define SM_PDOWN        	0x10   // ��λʱ�������ʡ��ģʽ
#define SM_TESTS        	0x20   //��λʱ����SDI����ģʽ
#define SM_STREAM       	0x40   //ʹ��vs103����ģʽ
#define SM_PLUSV        	0x80   //MP3+V����ʹ��
#define SM_DACT         	0x100   // ����sck�����ػ��½�����Ч
#define SM_SDIORD       	0x200   //����sdi�����ֽ�˳��
#define SM_SDISHARE     	0x400   //��λʱ��xDCS��ʡȥ��ֻ��xCS
#define SM_SDINEW       	0x800   //��λ������ģʽ
#define SM_ADPCM        	0x1000   //��λʱADPCM¼��ʹ��
#define SM_ADPCM_HP     	0x2000 //��λʱʹ��ADPCM��ͨ�˲���
#define SM_LINE_IN	        0x4000		// 1ѡ��line in��0ѡ����˷�

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
