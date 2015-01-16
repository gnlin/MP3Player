#include "stm32f10x.h"
#include "sdcard.h"
#include "spi.h"

u8 SD_Type = 0;

//等待SD卡回应
//Response:要得到的回应值
//返回值:0,成功得到了该回应值, 其他,得到回应值失败
u8 SD_GetResponse(u8 Response)
{
	u16 count = 0xFFF;
	while ((SPI_ReadByte(SPI2) != Response) && count) count--;
	if (count == 0)
		return MSD_RESPONSE_FAILURE;
	else
		return MSD_RESPONSE_NO_ERROR;
}

//等待SD卡写入完成
//返回值:0,成功
//       其他,错误代码
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

//向SD卡发送一个命令
//输入: u8 cmd   命令 
//      u32 arg  命令参数
//      u8 crc   crc校验值	   
//返回值:SD卡返回的响应	
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

//向SD卡发送一个命令(结束是不失能片选，还有后续数据传来）
//输入:u8 cmd   命令 
//     u32 arg  命令参数
//     u8 crc   crc校验值	 
//返回值:SD卡返回的响应	
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

//把SD卡设置到挂起模式
//返回值:0,成功设置
//       1,设置失败
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

//初始化SD卡
//如果成功返回,则会自动设置SPI速度为18Mhz
//返回值:0：NO_ERR
//       1：TIME_OUT
//       99：NO_CARD
u8 SD_Init(void)
{
	u8 R1;
	u16 retry;
	u8 buffer[4];

    SPI2_Init();
	SPI_SetSpeed(SPI2, SPI_SPEED_256);
	Set_SD_CS();

	if (SD_Idle_Sta()) return 1;
	//-----------------SD卡复位到idle结束-----------------	 
	//获取卡片的SD版本信息
	Clr_SD_CS();
	R1 = SD_SendCommand_NoDeassert(8, 0x1AA, 0x87);
	if (R1 == 0x05) {
		SD_Type = SD_TYPE_V1;
	    Set_SD_CS();
		SPI_WriteByte(SPI2, 0xFF);
 		//-----------------SD卡、MMC卡初始化开始-----------------	 
		//发卡初始化指令CMD55+ACMD41
		// 如果有应答，说明是SD卡，且初始化完成
		// 没有回应，说明是MMC卡，额外进行相应初始化
		retry = 0;
		do {
			R1 = SD_SendCommand(CMD55, 0, 0);
			if (R1 == 0xFF) return R1;
			R1 = SD_SendCommand(ACMD41, 0, 0);
			retry++;
		} while ((R1 != MSD_RESPONSE_NO_ERROR) && (retry < 400));
		// 判断是超时还是得到正确回应
		// 若有回应：是SD卡；没有回应：是MMC卡	  
		//----------MMC卡额外初始化操作开始------------
		if (retry >= 400) {
			retry = 0;
			do {
				R1 = SD_SendCommand(CMD1, 0, 0);
				retry++;
			} while ((R1 != MSD_RESPONSE_NO_ERROR) && (retry < 400));
			if (retry >= 400) return 1;
			SD_Type = SD_TYPE_MMC;
		}
		//----------MMC卡额外初始化操作结束------------	    
		//设置SPI为高速模式
	    SPI_SetSpeed(SPI2, SPI_SPEED_4);
		SPI_WriteByte(SPI2, 0xFF);
		R1 = SD_SendCommand(CMD59, 0, 0x95);
		if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
		//设置Sector Size
		R1 = SD_SendCommand(CMD16, 512, 0x95);
		if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
		//-----------------SD卡、MMC卡初始化结束-----------------
	} //SD卡为V1.0版本的初始化结束	
	//下面是V2.0卡的初始化
	//其中需要读取OCR数据，判断是SD2.0还是SD2.0HC卡
	else if (R1 == 0x01) {
		//V2.0的卡，CMD8命令后会传回4字节的数据，要跳过再结束本命令
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
		//初始化指令发送完成，接下来获取OCR信息		   
		//-----------鉴别SD2.0卡版本开始-----------
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
		//检查接收到的OCR中的bit30位（CCS），确定其为SD2.0还是SDHC
		//如果CCS=1：SDHC   CCS=0：SD2.0
		if (buffer[0] & 0x40)
		{
			SD_Type = SD_TYPE_V2HC;
		}
		else
		{
			SD_Type = SD_TYPE_V2;
		}
		//-----------鉴别SD2.0卡版本结束----------- 
		//设置SPI为高速模式	
		SPI_SetSpeed(SPI2, SPI_SPEED_4);					
	}
	return R1;
}

//从SD卡中读回指定长度的数据，放置在给定位置
//输入: u8 *data(存放读回数据的内存>len)
//      u16 len(数据长度）
//      u8 release(传输完成后是否释放总线CS置高 0：不释放 1：释放）	 
//返回值:0：NO_ERR
//  	 other：错误信息
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

//获取SD卡的CID信息，包括制造商信息
//输入: u8 *cid_data(存放CID的内存，至少16Byte）	  
//返回值:0：NO_ERR
//		 1：TIME_OUT
//       other：错误信息
u8 SD_GetCID(u8 *cid_data)
{
	u8 R1;
	R1 = SD_SendCommand(CMD10, 0, 0xFF);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;
	SD_ReceiveData(cid_data, 16, RELEASE);
	return 0;
}

//获取SD卡的CSD信息，包括容量和速度信息
//输入:u8 *cid_data(存放CID的内存，至少16Byte）	    
//返回值:0：NO_ERR
//       1：TIME_OUT
//       other：错误信息														   
u8 SD_GetCSD(u8 *csd_data)
{
	u8 R1;	 
	R1 = SD_SendCommand(CMD9, 0, 0xFF);
	if (R1 != MSD_RESPONSE_NO_ERROR) return R1;   
	SD_ReceiveData(csd_data, 16, RELEASE);
	return 0;
}  

//获取SD卡的容量（字节）   
//返回值:0： 取容量出错 
//       其他:SD卡的容量(字节)
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

//读SD卡的一个block
//输入:u32 sector 取地址（sector值，非物理地址） 
//     u8 *buffer 数据存储地址（大小至少512byte） 		   
//返回值:0： 成功
//       other：失败
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

//写入SD卡的一个block									    
//输入:u32 sector 扇区地址（sector值，非物理地址） 
//     u8 *buffer 数据存储地址（大小至少512byte） 		   
//返回值:0： 成功
//       other：失败
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

//读SD卡的多个block										    
//输入:u32 sector 扇区地址（sector值，非物理地址） 
//     u8 *buffer 数据存储地址（大小至少512byte）
//     u8 count 连续读count个block 		   
//返回值:0： 成功
//       other：失败
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

//写入SD卡的多个block									    
//输入:u32 sector 扇区地址（sector值，非物理地址） 
//     u8 *buffer 数据存储地址（大小至少512byte）
//     u8 count 写入的block数目		   
//返回值:0： 成功
//       other：失败
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

//在指定扇区,从offset开始读出bytes个字节								    
//输入:unsigned long address: 物理地址
//     u8 *buf     数据存储地址（大小<=512byte）
//     u16 offset  在扇区里面的偏移量
//     u16 bytes   要读出的字节数	   
//返回值:0： 成功
//       other：失败
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
