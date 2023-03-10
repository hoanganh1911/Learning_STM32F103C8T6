#include "w25q_spi.h"
uint8_t buf[256];
//---------------------------------------------------------
/**
 * @brief Send data to W25Qxx
 *
 * @param hspi
 * @param dt
 * @param cnt
 */
void W25_Send(SPI_HandleTypeDef hspi,uint8_t *dt,uint16_t cnt)
{
	HAL_SPI_Transmit(&hspi, dt, cnt, 1000);
}
//---------------------------------------------------------
/**
 * @brief Receive data from W25Qxx
 *
 * @param hspi
 * @param dt
 * @param cnt
 */
void W25_Recv(SPI_HandleTypeDef hspi,uint8_t *dt,uint16_t cnt)
{
	HAL_SPI_Receive(&hspi, dt, cnt, 1000);
}
//---------------------------------------------------------
/**
 * @brief Send reset command W25Qxx
 *
 * @param hspi
 */
void W25_Reset(SPI_HandleTypeDef hspi)
{
	cs_set();
	buf[0] = W25_ENABLE_RESET;
	buf[1] = W25_RESET;
	W25_Send(hspi,buf, 2);
	cs_reset();
}
//---------------------------------------------------------
/**
 * @brief Read data from W25Qxx
 *
 * @param hspi
 * @param addr
 * @param data
 * @param sz
 */
void W25_Read_Data(SPI_HandleTypeDef hspi,uint32_t addr,uint8_t* data,uint32_t sz)
{
	cs_set();
	buf[0] = W25_READ;
	buf[1] = (addr >> 16 ) & 0xFF;
	buf[2] = (addr >> 8 ) & 0xFF;
	buf[3] =  addr & 0xFF;
	W25_Send(hspi,buf, 4);
	W25_Recv(hspi,data, sz);
	cs_reset();
}
//---------------------------------------------------------
/**
 * @brief Read Unique ID from W25Qxx
 *
 * @param hspi
 * @return
 */
uint64_t W25_Read_ID(SPI_HandleTypeDef hspi)
{
	uint8_t dt[12];
	buf[0] = W25_UNIQUE_ID;
	cs_set();
	W25_Send(hspi,buf, 1);
	W25_Recv(hspi,dt,12);
	cs_reset();
	uint64_t received_id = 0;
	for (int i = 4;i < 12;i++)
	{
		received_id |= dt[i];
		if (i!=11)
			received_id <<= 8;
	}
	return received_id;
}
//---------------------------------------------------------
void UART_64bit(UART_HandleTypeDef huart,uint64_t x)
{
	char str1[100];
	char str2[100];
	uint32_t low = x;
	uint32_t high = (x >> 32);
	sprintf(str1,"ID:0x%lX",high);
	sprintf(str2,"%lX\r\n",low);
	strcat(str1,str2);
	HAL_UART_Transmit(&huart,(uint8_t *) str1,strlen(str1),1000);
}
//---------------------------------------------------------
/**
 * @brief Read data by Page
 *
 * @param hspi
 * @param w25_info
 * @param data
 * @param page_addr
 * @param offset
 * @param sz
 */
void W25_Read_Page(SPI_HandleTypeDef hspi,w25_info_t w25_info,uint8_t* data, uint32_t page_addr, uint32_t offset, uint32_t sz)
{
	if(sz > w25_info.PageSize)
		sz = w25_info.PageSize;
	if((offset+sz) > w25_info.PageSize)
		sz = w25_info.PageSize - offset;
	page_addr = page_addr*w25_info.PageSize + offset;
	buf[0] = W25_FAST_READ;
	if(w25_info.high_cap)
	{
		buf[1] = (page_addr >> 24) & 0xFF;
		buf[2] = (page_addr >> 16) & 0xFF;
		buf[3] = (page_addr >> 8) & 0xFF;
		buf[4] = page_addr & 0xFF;
		buf[5] = 0;
		cs_set();
		W25_Send(hspi,buf,6);
	}
	else
	{
		buf[1] = (page_addr >> 16) & 0xFF;
		buf[2] = (page_addr >> 8) & 0xFF;
		buf[3] = page_addr & 0xFF;
		buf[4] = 0;
		cs_set();
		W25_Send(hspi,buf, 5);
	}
	W25_Recv(hspi,data, sz);
	cs_reset();
}
//---------------------------------------------------------
void W25_PrintUart_ReadData(SPI_HandleTypeDef hspi,UART_HandleTypeDef huart)
{
	char str1[30];
	unsigned int addr=0;
	for(uint16_t k=0; k<10; k++) // ??ang ?????c 4 trang n???u mu???n ?????c h???t s??? = 4194304/256 = 16384
	{
		//W25_Read_Page(buf,k,0,256);
		W25_Read_Data(hspi,k*256, buf, 256); // M???i l???n ?????c v??? s??? ?????c 256 bytes t????ng ???ng v???i 256 ?????a ch???
		for(uint8_t i=0; i<16; i++) // S??? h??ng
		{
			addr = k*256 + i*16;
			sprintf(str1,"%08X: ", addr);
			HAL_UART_Transmit(&huart,(uint8_t*)str1,10,0x1000);
			for(uint8_t j=0; j<16; j++) // S??? c???t
			{
				sprintf(str1,"%02X", buf[(uint16_t)i*16 + (uint16_t)j]); // addr = s??? h??ng *16 + s??? c???t
				HAL_UART_Transmit(&huart,(uint8_t*)str1,2,0x1000); // In ra m?? HEX t???i ?????a ch??? t????ng ???ng
				if(j==7)
					HAL_UART_Transmit(&huart,(uint8_t*)"|",1,0x1000);
				else
					HAL_UART_Transmit(&huart,(uint8_t*)" ",1,0x1000);
			}
			HAL_UART_Transmit(&huart,(uint8_t*)"| ",1,0x1000);
			for(uint8_t j=0; j<16; j++)
			{
				if ((buf[(uint16_t)i*16 + (uint16_t)j] == 0x0A) || (buf[(uint16_t)i*16 + (uint16_t)j] == 0x0D))
					sprintf(str1," ");
				else
					sprintf(str1,"%c", (char) buf[(uint16_t)i*16 + (uint16_t)j]);
				HAL_UART_Transmit(&huart,(uint8_t*)str1,1,0x1000); // In k?? t??? t???i ?????a ch??? t????ng ???ng
			}
			HAL_UART_Transmit(&huart,(uint8_t*)"\r\n",2,0x1000);
		}
		HAL_UART_Transmit(&huart,(uint8_t*)"\r\n",2,0x1000);
	}
}
//---------------------------------------------------------
/**
 * @brief Read Jedec ID W25Qxx
 *
 * @param hspi
 * @return
 */
uint32_t W25_Read_Jedec_ID(SPI_HandleTypeDef hspi)
{
	uint8_t dt[4];
	buf[0] = W25_GET_JEDEC_ID;
	cs_set();
	W25_Send(hspi,buf, 1);
	W25_Recv(hspi,dt,3);
	cs_reset();
	return (dt[0] << 16) | (dt[1] << 8) | dt[2];
}
//---------------------------------------------------------
void W25_Info(SPI_HandleTypeDef hspi,UART_HandleTypeDef huart,w25_info_t *w25_info)
{
	uint32_t Jedec_ID = W25_Read_Jedec_ID(hspi);
	Jedec_ID &= 0x0000ffff;
	w25_info->high_cap = 0;
	char str[30];
	switch(Jedec_ID)
	{
	case 0x401A:
		w25_info->high_cap = 1;
		w25_info->BlockCount=1024;
		sprintf(str,"w25qxx Chip: w25q512\r\n");
		break;
	case 0x4019:
		w25_info->high_cap = 1;
		w25_info->BlockCount=512;
		sprintf(str,"w25qxx Chip: w25q256\r\n");
		break;
	case 0x4018:
		w25_info->BlockCount=256;
		sprintf(str,"w25qxx Chip: w25q128\r\n");
		break;
	case 0x4017:
		w25_info->BlockCount=128;
		sprintf(str,"w25qxx Chip: w25q64\r\n");
		break;
	case 0x4016:
		w25_info->BlockCount=64;
		sprintf(str,"w25qxx Chip: w25q32\r\n");
		break;
	case 0x4015:
		w25_info->BlockCount=32;
		sprintf(str,"w25qxx Chip: w25q16\r\n");
		break;
	case 0x4014:
		w25_info->BlockCount=16;
		sprintf(str,"w25qxx Chip: w25q80\r\n");
		break;
	case 0x4013:
		w25_info->BlockCount=8;
		sprintf(str,"w25qxx Chip: w25q40\r\n");
		break;
	case 0x4012:
		w25_info->BlockCount=4;
		sprintf(str,"w25qxx Chip: w25q20\r\n");
		break;
	case 0x4011:
		w25_info->BlockCount=2;
		sprintf(str,"w25qxx Chip: w25q10\r\n");
		break;
	default:
		sprintf(str,"w25qxx Unknown ID\r\n");
		break;
	}
	w25_info->PageSize=256;
	w25_info->SectorSize=0x1000;
	w25_info->SectorCount=w25_info->BlockCount*16;
	w25_info->PageCount=(w25_info->SectorCount*w25_info->SectorSize)/w25_info->PageSize;
	w25_info->BlockSize=w25_info->SectorSize*16;
	w25_info->NumKB=(w25_info->SectorCount*w25_info->SectorSize)/1024;
}
//---------------------------------------------------------
/**
 * @brief Inint W25Qxx
 *
 * @param hspi
 */
void W25_Init(SPI_HandleTypeDef hspi)
{
	HAL_Delay(100);
	W25_Reset(hspi);
	HAL_Delay(100);
}
//---------------------------------------------------------
/**
 * @brief Enable Write data to W25Qxx
 *
 * @param hspi
 */
void W25_Write_Enable(SPI_HandleTypeDef hspi)
{
	cs_set();
    buf[0] = W25_WRITE_ENABLE;
	W25_Send(hspi,buf, 1);
	cs_reset();
}
//-------------------------------------------------------------
/**
 * @brief Disable Write data to W25Qxx
 *
 * @param hspi
 */
void W25_Write_Disable(SPI_HandleTypeDef hspi)
{
	cs_set();
    buf[0] = W25_WRITE_DISABLE;
	W25_Send(hspi,buf, 1);
	cs_reset();
}
//-------------------------------------------------------------
/**
 * @brief Check busy bit in register 1
 *
 * @param hspi
 * @param w25_info
 */
void W25_Wait_Write_End(SPI_HandleTypeDef hspi,w25_info_t w25_info)
{
	HAL_Delay(1);
	cs_set();
	buf[0] = W25_READ_STATUS_1;
	W25_Send(hspi,buf, 1);
	do{
		W25_Recv(hspi,buf,1);
		w25_info.StatusRegister1 = buf[0];
		HAL_Delay(1);
	}
	while((w25_info.StatusRegister1 & 0x01) == 0x01);
	cs_reset();
}
//-------------------------------------------------------------
/**
 * @brief Protect Writting to W25Qxx
 *
 * @param hspi
 * @param val
 */
void W25_Set_Block_Protect(SPI_HandleTypeDef hspi,uint8_t val)
{
	buf[0] = 0x50;
	cs_set();
	W25_Send(hspi,buf, 1);
	cs_reset();
	buf[0] = W25_WRITE_STATUS_1;
	buf[1] = ((val & 0x0F) << 2);
	cs_set();
	W25_Send(hspi,buf, 2);
	cs_reset();
}
//-------------------------------------------------------------
/**
 * @brief Write data to W25Qxx
 *
 * @param hspi
 * @param w25_info
 * @param addr
 * @param data
 * @param sz
 */
void W25_Write_Data(SPI_HandleTypeDef hspi,w25_info_t w25_info,uint32_t addr, uint8_t* data, uint32_t sz)
{
	W25_Wait_Write_End(hspi,w25_info);
	W25_Set_Block_Protect(hspi,0x00);
	W25_Write_Enable(hspi);
	cs_set();
	buf[0] = W25_PAGE_PROGRAMM;
	if(w25_info.high_cap)
	{
		buf[1] = (addr >> 24) & 0xFF;
		buf[2] = (addr >> 16) & 0xFF;
		buf[3] = (addr >> 8) & 0xFF;
		buf[4] = addr & 0xFF;
		W25_Send(hspi,buf, 5);
	}
	else
	{
		buf[1] = (addr >> 16) & 0xFF;
		buf[2] = (addr >> 8) & 0xFF;
		buf[3] = addr & 0xFF;
		W25_Send(hspi,buf, 4);
	}
	W25_Send(hspi,data, sz);
	cs_reset();
	W25_Wait_Write_End(hspi,w25_info);
	W25_Write_Disable(hspi);
	W25_Set_Block_Protect(hspi,0x0F);
}
//-------------------------------------------------------------
/**
 * @brief Write data page by page to W25Qxx
 *
 * @param hspi
 * @param w25_info
 * @param data
 * @param page_addr
 * @param offset
 * @param sz
 */
void W25_Write_Page(SPI_HandleTypeDef hspi,w25_info_t w25_info,uint8_t* data, uint32_t page_addr, uint32_t offset, uint32_t sz)
{
	if(sz > w25_info.PageSize)
		sz = w25_info.PageSize;
	if((offset+sz) > w25_info.PageSize)
		sz = w25_info.PageSize - offset;
	page_addr = page_addr * w25_info.PageSize + offset;

	W25_Wait_Write_End(hspi,w25_info);
	W25_Set_Block_Protect(hspi,0x00);
	W25_Write_Enable(hspi);
	cs_set();
	buf[0] = W25_PAGE_PROGRAMM;
	if(w25_info.high_cap)
	{
		buf[1] = (page_addr >> 24) & 0xFF;
		buf[2] = (page_addr >> 16) & 0xFF;
		buf[3] = (page_addr >> 8) & 0xFF;
		buf[4] = page_addr & 0xFF;
		W25_Send(hspi,buf, 5);
	}
	else
	{
		buf[1] = (page_addr >> 16) & 0xFF;
		buf[2] = (page_addr >> 8) & 0xFF;
		buf[3] = page_addr & 0xFF;
		W25_Send(hspi,buf, 4);
	}
	W25_Send(hspi,data, sz);
	cs_reset();
	W25_Wait_Write_End(hspi,w25_info);
	W25_Write_Disable(hspi);
	W25_Set_Block_Protect(hspi,0x0F);
}
//-------------------------------------------------------------
/**
 * @brief The Sector Erase instruction sets all memory within a specified sector (4K-bytes) to the erased state of all 1s (FFh)
 *
 * @param hspi
 * @param w25_info
 * @param addr
 */
void W25_Erase_Sector(SPI_HandleTypeDef hspi,w25_info_t w25_info,uint32_t addr)
{
	W25_Wait_Write_End(hspi,w25_info);
	W25_Set_Block_Protect(hspi,0x00);
	addr = addr * w25_info.SectorSize;
	W25_Write_Enable(hspi);
	cs_set();
	buf[0] = W25_SECTOR_ERASE;
	if(w25_info.high_cap)
	{
		buf[1] = (addr >> 24) & 0xFF;
		buf[2] = (addr >> 16) & 0xFF;
		buf[3] = (addr >> 8) & 0xFF;
		buf[4] = addr & 0xFF;
		W25_Send(hspi,buf, 5);
	}
	else
	{
		buf[1] = (addr >> 16) & 0xFF;
		buf[2] = (addr >> 8) & 0xFF;
		buf[3] = addr & 0xFF;
		W25_Send(hspi,buf, 4);
	}
	cs_reset();
	W25_Wait_Write_End(hspi,w25_info);
	W25_Write_Disable(hspi);
	W25_Set_Block_Protect(hspi,0x0F);
}
//-------------------------------------------------------------
/**
 * @brief The Block Erase instruction sets all memory within a specified block (64K-bytes) to the erased state of all 1s (FFh).
 *
 * @param hspi
 * @param w25_info
 * @param addr
 */
void W25_Erase_Block(SPI_HandleTypeDef hspi,w25_info_t w25_info,uint32_t addr)
{
  W25_Wait_Write_End(hspi,w25_info);
  W25_Set_Block_Protect(hspi,0x00);
  addr = addr * w25_info.BlockSize;
  W25_Write_Enable(hspi);
  cs_set();
  buf[0] = W25_BLOCK_ERASE;
  if(w25_info.high_cap)
  {
    buf[1] = (addr >> 24) & 0xFF;
    buf[2] = (addr >> 16) & 0xFF;
    buf[3] = (addr >> 8) & 0xFF;
    buf[4] = addr & 0xFF;
    W25_Send(hspi,buf, 5);
  }
  else
  {
    buf[1] = (addr >> 16) & 0xFF;
    buf[2] = (addr >> 8) & 0xFF;
    buf[3] = addr & 0xFF;
    W25_Send(hspi,buf, 4);
  }
  cs_reset();
  W25_Wait_Write_End(hspi,w25_info);
	W25_Write_Disable(hspi);
	W25_Set_Block_Protect(hspi,0x0F);
}
//-------------------------------------------------------------
/**
 * @brief The Chip Erase instruction sets all memory within the device to the erased state of all 1s (FFh)
 *
 * @param hspi
 * @param w25_info
 */
void W25_Erase_Chip(SPI_HandleTypeDef hspi,w25_info_t w25_info)
{
	W25_Wait_Write_End(hspi,w25_info);
	W25_Set_Block_Protect(hspi,0x00);
	W25_Write_Enable(hspi);
	cs_set();
	buf[0] = W25_CHIP_ERASE;
	W25_Send(hspi,buf, 1);
	cs_reset();
	W25_Wait_Write_End(hspi,w25_info);
	W25_Write_Disable(hspi);
	W25_Set_Block_Protect(hspi,0x0F);
}
//-------------------------------------------------------------
