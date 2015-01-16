#include "stm32f10x.h"
#include "sdcard.h"
#include "spi.h"

u8 SD_Type = 0;

//�ȴ�SD����Ӧ
//Response:Ҫ�õ��Ļ�Ӧֵ
//����ֵ:0,�ɹ��õ��˸û�Ӧֵ, ����,�õ���Ӧֵʧ��
u8 SD_GetResponse(u8 Response)
{
	u16 count = 0xFFF;
	while ((SPI_ReadByte(SPI2) != Response) && count) count--;
	if (count == 0)
		return MSD_RESPONSE_FAILURE;
	else
		return MSD_RESPONSE_NO_ERROR;
}

//�ȴ�SD��д�����
//����ֵ:0,�ɹ�
//       ����,�������
u8 SD_WaitDataReady(void)
{
	u8 R1 = MSD_DATA_OTHER_ERROR;
	u32 retry = 0;
	do {
		R1 = SPI_ReadByte(SPI2) & 0x1F;
		if (retry == 0xFFE) return 1;
		retry++;
		switch (R1)
		{
			case MSD_DATA_OK:
				break;
			case MSD_DATA_CRC_ERROR:
				return MSD_DATA_CRC_ERROR;
			case MSD_DATA_WRITE_ERROR:
				return MSD_DATA_WRITE_ERROR;
			default:
				R1 = MSD_DATA_OTHER_ERROR;
		}
	} while (R1 == MSD_DATA_OTHER_ERROR);
	retry = 0;
	while (SPI_ReadByte(SPI2) == 0) {
		retry++;
		if (retry >= 0xFFFFFFFE) return 0xFF;
	}
	return 0;
}

//��SD������һ������
//����: u8 cmd   ���� 
//      u32 arg  �������
//      u8 crc   crcУ��ֵ	   
//����ֵ:SD�����ص���Ӧ	
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc)
{
	u8 R1;
	u8 retry = 0;
	Set_SD_CS();			   
	SPI_WriteByte(SPI2, 0xFF);
	SPI_WriteByte(SPI2, 0xFF);
	SPI_WriteByte(SPI2, 0xFF);
	Clr_SD_CS();

	SPI_WriteByte(SPI2, cmd | 0x40);
	SPI_WriteByte(SPI2, arg >> 24);
	SPI_WriteByte(SPI2, arg >> 16);
	SPI_WriteByte(SPI2, arg >> 8);
	SPI_WriteByte(SPI2, arg);
	SPI_WriteByte(SPI2, crc);

	while ((R1 = SPI_WriteByte(SPI2, 0xFF)) == 0xFF) {
		retry++;
		if (retry >= 200) break;
	}
	Set_SD_CS();
	SPI_WriteByte(SPI2, 0xFF);
	return R1;
}

//��SD������һ������(�����ǲ�ʧ��Ƭѡ�����к������ݴ�����
//����:u8 cmd   ���� 
//     u32 arg  �������
//     u8 crc   crcУ��ֵ	 
//����ֵ:SD�����ص���Ӧ	
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg, u8 crc)
{
	u8 retry = 0;	         
	u8 R1;
	Set_SD_CS();			   
	SPI_WriteByte(SPI2, 0xFF);
	SPI_WriteByte(SPI2, 0xFF);
	SPI_WriteByte(SPI2, 0xFF);	 	 	 
	Clr_SD_CS();   
	
	SPI_WriteByte(SPI2, cmd | 0x40);
	SPI_WriteByte(SPI2, arg >> 24);
	SPI_WriteByte(SPI2, arg >> 16);
	SPI_WriteByte(SPI2, arg >> 8);
	SPI_WriteByte(SPI2, arg);
	SPI_WriteByte(SPI2, crc); 

	while ((R1 = SPI_WriteByte(SPI2, 0xFF)) == 0xFF) {
		retry++;
		if (retry >= 200) break;
	}	  

	return R1;
}

//��SD�����õ�����ģʽ
//����ֵ:0,�ɹ�����
//       1,����ʧ��
u8 SD_Idle_Sta(void)
{
	u16 i;
	u8 retry;
	for (i = 0; i > 0xF00; i++);
	for (i = 0; i < 10; i++)
		SPI_ReadByte(SPI2);

	retry = 0;
	do {
		i = SD_SendCommand(CMD0, 0, 0x95);
		retry++;	
	} while ((i != MSD_IN_IDLE_STATE) && (retry < 200));
	if (retry >= 200) return 1;
	return 0;
}

//��ʼ��SD��
//����ɹ�����,����Զ�����SPI�ٶ�Ϊ18Mhz
//����ֵ:0��NO_ERR
//       1��TIME_OUT
//       99��NO_CARD
u8 SD_Init(void)
{
	u8 R1;
	u16 retry;
	u8 buffer[4];

    SPI2_Init();
	SPI_SetSpeed(SPI2, SPI_SPEED_256);
	Set_SD_CS();

	if (SD_Idle_Sta()) return 1;
	//-----------------SD����λ��idle����-----------------	 
	//��ȡ��Ƭ��SD�汾��Ϣ
	Clr_SD_CS();
	R1 = SD_SendCommand_NoDeassert(8, 0x1AA, 0x87);
	if (R1 == 0x05) {
		SD_Type = SD_TYPE_V1;
	    Set_SD_CS();
		SPI_WriteByte(SPI2, 0xFF);
 		//-----------------SD����MMC����ʼ����ʼ-----------------	 
		//������ʼ��ָ��CMD55+ACMD41
		// �����Ӧ��˵����SD�����ҳ�ʼ�����
		// û�л�Ӧ��˵����MMC�������������Ӧ��ʼ��
		retry = 0;
		do {
			R1 = SD_SendCommand(CMD55, 0, 0);
			if (R1 == 0xFF) return R1;
			R1 = SD_SendCommand(ACMD41, 0, 0);
			retry++;
		} while ((R1 != MSD_RESPONSE_NO_ERROR) && (retry < 400));
		// �ж��ǳ�ʱ���ǵõ���ȷ��Ӧ
		// ���л�Ӧ����SD����û�л�Ӧ����MMC��	  
		//----------MMC�������ʼ��������ʼ------------
		if (retry >= 400) {
			retry = 0;
			do {
				R1 = SD_SendCommand(CMD1, 0, 0);
				retry++;
			} while ((R1 != MSD_RESPONSE_NO_ERROR) && (retry < 400));
			if (retry >= 400) return 1;
			SD_Type = SD_TYPE_MMC;
		}
		//----------MMC�������ʼ����������------------	    
		//����SPIΪ����ģʽ
	    SPI_SetSpeed(SPI2, SPI_SPEED_4);
		SPI_WriteByte(SPI2, 0xFF);
		R1 = SD_SendCommand(CMD59, 0, 0x95);
		if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
		//����Sector Size
		R1 = SD_SendCommand(CMD16, 512, 0x95);
		if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
		//-----------------SD����MMC����ʼ������-----------------
	} //SD��ΪV1.0�汾�ĳ�ʼ������	
	//������V2.0���ĳ�ʼ��
	//������Ҫ��ȡOCR���ݣ��ж���SD2.0����SD2.0HC��
	else if (R1 == 0x01) {
		//V2.0�Ŀ���CMD8�����ᴫ��4�ֽڵ����ݣ�Ҫ�����ٽ���������
		buffer[0] = SPI_WriteByte(SPI2, 0xFF);   // 0x00
	  	buffer[1] = SPI_WriteByte(SPI2, 0xFF);	 // 0x00
		buffer[2] = SPI_WriteByte(SPI2, 0xFF);	 // 0x01
		buffer[3] = SPI_WriteByte(SPI2, 0xFF);	 // 0xAA
		Set_SD_CS();
		SPI_ReadByte(SPI2);
		retry = 0;
		do {
			R1 = SD_SendCommand(CMD55, 0, 0);
			if (R1 != 0x01) return R1;
			R1 = SD_SendCommand(ACMD41, 0x40000000, 0);
			retry++;	
		} while ((R1 != MSD_RESPONSE_NO_ERROR) && (retry < 400));
	    if (retry >= 400) return R1;
		//��ʼ��ָ�����ɣ���������ȡOCR��Ϣ		   
		//-----------����SD2.0���汾��ʼ-----------
		R1 = SD_SendCommand_NoDeassert(CMD58, 0, 0);
		if (R1 != MSD_RESPONSE_NO_ERROR) {
			Set_SD_CS();
			return R1;
		}
		buffer[0] = SPI_ReadByte(SPI2);
		buffer[1] = SPI_ReadByte(SPI2);
		buffer[2] = SPI_ReadByte(SPI2);
		buffer[3] = SPI_ReadByte(SPI2);		
		Set_SD_CS();
		SPI_ReadByte(SPI2);
		//�����յ���OCR�е�bit30λ��CCS����ȷ����ΪSD2.0����SDHC
		//���CCS=1��SDHC   CCS=0��SD2.0
		if (buffer[0] & 0x40)
		{
			SD_Type = SD_TYPE_V2HC;
		}
		else
		{
			SD_Type = SD_TYPE_V2;
		}
		//-----------����SD2.0���汾����----------- 
		//����SPIΪ����ģʽ	
		SPI_SetSpeed(SPI2, SPI_SPEED_4);					
	}
	return R1;
}

//��SD���ж���ָ�����ȵ����ݣ������ڸ���λ��
//����: u8 *data(��Ŷ������ݵ��ڴ�>len)
//      u16 len(���ݳ��ȣ�
//      u8 release(������ɺ��Ƿ��ͷ�����CS�ø� 0�����ͷ� 1���ͷţ�	 
//����ֵ:0��NO_ERR
//  	 other��������Ϣ
u8 SD_ReceiveData(u8 *data, u16 len, u8 release)
{
	Clr_SD_CS();
	if (SD_GetResponse(0xFE)) {
		Set_SD_CS();
		return 1;
	}
	while (len--) {
		*data = SPI_ReadByte(SPI2);
		data++;
	}
	// dummy CRC
	SPI_ReadByte(SPI2);
	SPI_ReadByte(SPI2);
	
	if (release == RELEASE) {
		Set_SD_CS();
		SPI_ReadByte(SPI2);
	}
	return 0;	
}

//��ȡSD����CID��Ϣ��������������Ϣ
//����: u8 *cid_data(���CID���ڴ棬����16Byte��	  
//����ֵ:0��NO_ERR
//		 1��TIME_OUT
//       other��������Ϣ
u8 SD_GetCID(u8 *cid_data)
{
	u8 R1;
	R1 = SD_SendCommand(CMD10, 0, 0xFF);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
	SD_ReceiveData(cid_data, 16, RELEASE);
	return 0;
}

//��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
//����:u8 *cid_data(���CID���ڴ棬����16Byte��	    
//����ֵ:0��NO_ERR
//       1��TIME_OUT
//       other��������Ϣ														   
u8 SD_GetCSD(u8 *csd_data)
{
	u8 R1;	 
	R1 = SD_SendCommand(CMD9, 0, 0xFF);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;   
	SD_ReceiveData(csd_data, 16, RELEASE);
	return 0;
}  

//��ȡSD�����������ֽڣ�   
//����ֵ:0�� ȡ�������� 
//       ����:SD��������(�ֽ�)
u32 SD_GetCapacity(void)
{
	u8 csd[16];
	u32 capacity;
	u8 R1;
	u16 i;
	u32 temp;

	if (SD_GetCSD(csd) != 0) return 0;
	if ((csd[0] & 0xC0) == 0x40) {
		capacity = ((u32)csd[8]) << 8;
		capacity += (u32)csd[9] + 1;
		capacity = (capacity)*1024;
		capacity *= 512;	
	} else {
		i = csd[6] & 0x03;
		i <<= 8;
		i += csd[7];
		i <<= 2;
		i += ((csd[8] & 0xC0) >> 6);
		R1 = csd[9] & 0x03;
		R1 <<= 1;
		R1 += ((csd[10] & 0x80) >> 7);
		R1 += 2;
		temp = 1;
		while (R1) {
			temp *= 2;
			R1--;
		}
		capacity = ((u32)(i + 1))*((u32)temp);
		i = csd[5] & 0x0F;
		temp = 1;
		while (i) {
			temp *= 2;
			i--;
		}			
		capacity *= (u32)temp;		
	}
	return capacity;
}

//��SD����һ��block
//����:u32 sector ȡ��ַ��sectorֵ���������ַ�� 
//     u8 *buffer ���ݴ洢��ַ����С����512byte�� 		   
//����ֵ:0�� �ɹ�
//       other��ʧ��
u8 SD_ReadSingleBlock(u32 sector, u8 *buffer)
{
	u8 R1;
	SPI_SetSpeed(SPI2, SPI_SPEED_4);
	if (SD_Type != SD_TYPE_V2HC) {
		sector = sector << 9;
	}
	R1 = SD_SendCommand(CMD17, sector, 0);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
	R1 = SD_ReceiveData(buffer, 512, RELEASE);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
	return 0;
}

//д��SD����һ��block									    
//����:u32 sector ������ַ��sectorֵ���������ַ�� 
//     u8 *buffer ���ݴ洢��ַ����С����512byte�� 		   
//����ֵ:0�� �ɹ�
//       other��ʧ��
u8 SD_WriteSingleBlock(u32 sector, const u8 *data)
{
	u8 R1;
	u16 i, retry;

	if (SD_Type != SD_TYPE_V2HC) {
		sector = sector << 9;
	}
	R1 = SD_SendCommand(CMD24, sector, 0x00);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
	Clr_SD_CS();
	SPI_ReadByte(SPI2);
	SPI_ReadByte(SPI2);
	SPI_ReadByte(SPI2);
	SPI_WriteByte(SPI2, 0xFE);

	for (i = 0; i < 512; i++)
		SPI_WriteByte(SPI2, *data++);
	
	SPI_ReadByte(SPI2);
	SPI_ReadByte(SPI2);

	R1 = SPI_ReadByte(SPI2);
	if ((R1 & 0x1F) != 0x05) {
		Set_SD_CS();
		return R1;
	}
	retry = 0;
	while (!SPI_ReadByte(SPI2)) {
		retry++;
		if (retry > 0xFFFE) {
			Set_SD_CS();
			return 1;	
		}
	}
	Set_SD_CS();
	SPI_ReadByte(SPI2);

	return 0;
}

//��SD���Ķ��block										    
//����:u32 sector ������ַ��sectorֵ���������ַ�� 
//     u8 *buffer ���ݴ洢��ַ����С����512byte��
//     u8 count ������count��block 		   
//����ֵ:0�� �ɹ�
//       other��ʧ��
u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count)
{
	u8 R1;
	if (SD_Type != SD_TYPE_V2HC)
		sector = sector << 9;
	R1 = SD_SendCommand(CMD18, sector, 0);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
	do {
		if (SD_ReceiveData(buffer, 512, NO_RELEASE) != 0x00) break;
		buffer += 512;
	} while (--count);

	SD_SendCommand(CMD12, 0, 0);
	Set_SD_CS();
	SPI_ReadByte(SPI2);
	if (count != 0) return count;
	return 0;
}

//д��SD���Ķ��block									    
//����:u32 sector ������ַ��sectorֵ���������ַ�� 
//     u8 *buffer ���ݴ洢��ַ����С����512byte��
//     u8 count д���block��Ŀ		   
//����ֵ:0�� �ɹ�
//       other��ʧ��
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count)
{
	u8 R1;
	u16 i;

	if (SD_Type != SD_TYPE_V2HC)
		sector = sector << 9;

	if (SD_Type != SD_TYPE_MMC)
		R1 = SD_SendCommand(ACMD23, count, 0x00);
	R1 = SD_SendCommand(CMD25, sector, 0x00);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
	Clr_SD_CS();

	SPI_ReadByte(SPI2);
	SPI_ReadByte(SPI2);

	do {
		SPI_ReadByte(SPI2);
		for (i = 0; i < 512; i++)
			SPI_WriteByte(SPI2, *data++);

		SPI_ReadByte(SPI2);
	    SPI_ReadByte(SPI2);	    
	    
		R1 = SPI_ReadByte(SPI2);
		if ((R1 & 0x1F) != 0x05) {
			Set_SD_CS();
			return R1;
		}

		if (SD_WaitDataReady() == 1) {
			Set_SD_CS();
			return 1;
		}
	} while (--count);

	R1 = SPI_WriteByte(SPI2, 0xFD);
	if (R1 == MSD_RESPONSE_NO_ERROR) count = 0xFE;
	if (SD_WaitDataReady()) {
		Set_SD_CS();
		return 1;
	}

	Set_SD_CS();
	SPI_ReadByte(SPI2);
	return count;
}	

//��ָ������,��offset��ʼ����bytes���ֽ�								    
//����:unsigned long address: �����ַ
//     u8 *buf     ���ݴ洢��ַ����С<=512byte��
//     u16 offset  �����������ƫ����
//     u16 bytes   Ҫ�������ֽ���	   
//����ֵ:0�� �ɹ�
//       other��ʧ��
u8 SD_Read_Bytes(unsigned long address, u8 *buf, u32 offset, u32 bytes)
{
	u8 R1;
	u16 i = 0;

	R1 = SD_SendCommand(CMD17, address << 9, 0);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
	Clr_SD_CS();

	if (SD_GetResponse(0xFE)) {
		Set_SD_CS();
		return 1;
	}

	for (i = 0; i < offset; i++)
		SPI_ReadByte(SPI2);

	for (; i < offset + bytes; i++)
		*buf++ = SPI_ReadByte(SPI2);

	for (; i < 512; i++)
		SPI_ReadByte(SPI2);

	SPI_ReadByte(SPI2);
	SPI_ReadByte(SPI2);

	Set_SD_CS();
	return 0;
}
