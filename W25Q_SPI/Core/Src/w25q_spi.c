#include "w25q_spi.h"
w25_info_t  w25_info;
uint8_t buf[10];
void SPI1_Send(uint8_t *dt,uint16_t cnt)
{
	HAL_SPI_Transmit(&hspi1, dt, cnt, 1000);
}
//---------------------------------------------------------
void SPI1_Recv(uint8_t *dt,uint16_t cnt)
{
	HAL_SPI_Receive(&hspi1, dt, cnt, 1000);
}
//---------------------------------------------------------
void W25_Reset(void)
{
	cs_set();
	buf[0] = W25_ENABLE_RESET;
	buf[1] = W25_RESET;
	SPI1_Send(buf, 2);
	cs_reset();
}
//---------------------------------------------------------
void W25_Read_Data(uint32_t addr,uint8_t* data,uint32_t sz)
{
	cs_set();
	buf[0] = W25_READ;
	buf[1] = (addr >> 16 ) & 0xFF;
	buf[2] = (addr >> 8 ) & 0xFF;
	buf[3] =  addr & 0xFF;
	SPI1_Send(buf, 4);
	SPI1_Recv(data, sz);
	cs_reset();
}
//---------------------------------------------------------
uint64_t W25_Read_ID(void)
{
	uint8_t dt[12];
	buf[0] = W25_UNIQUE_ID;
	cs_set();
	SPI1_Send(buf, 1);
	SPI1_Recv(dt,12);
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
void UART_64bit(uint64_t x)
{
	char str1[100];
	char str2[100];
	uint32_t low = x;
	uint32_t high = (x >> 32);
	sprintf(str1,"ID:0x%lX",high);
	sprintf(str2,"%lX\r\n",low);
	strcat(str1,str2);
	HAL_UART_Transmit(&huart1,(uint8_t *) str1,strlen(str1),1000);
}
//---------------------------------------------------------
void W25_Read_Page(uint8_t* data, uint32_t page_addr, uint32_t offset, uint32_t sz)
{
	if(sz > w25_info.PageSize)
		sz = w25_info.PageSize;
	if((offset+sz) > w25_info.PageSize)
		sz = w25_info.PageSize - offset;
	page_addr = page_addr*w25_info.PageSize + offset;
}
//---------------------------------------------------------
void W25_PrintUart_ReadData(void)
{
	char str1[30];
	unsigned int addr=0;
	for(uint16_t k=0; k<4; k++)
	{
		W25_Read_Data(k*256, buf, 256);
		for(uint8_t i=0; i<16; i++)
		{
			addr = k*256 + i*16;
			sprintf(str1,"%08X: ", addr);
			HAL_UART_Transmit(&huart1,(uint8_t*)str1,10,0x1000);
			for(uint8_t j=0; j<16; j++)
			{
				sprintf(str1,"%02X", buf[(uint16_t)i*16 + (uint16_t)j]);
				HAL_UART_Transmit(&huart1,(uint8_t*)str1,2,0x1000);
				if(j==7) HAL_UART_Transmit(&huart1,(uint8_t*)"|",1,0x1000);
				else HAL_UART_Transmit(&huart1,(uint8_t*)" ",1,0x1000);
			}
			HAL_UART_Transmit(&huart1,(uint8_t*)"| ",1,0x1000);
			for(uint8_t j=0; j<16; j++)
			{
				if ((buf[(uint16_t)i*16 + (uint16_t)j] == 0x0A) || (buf[(uint16_t)i*16 + (uint16_t)j] == 0x0D)) sprintf(str1," ");
				else sprintf(str1,"%c", (char) buf[(uint16_t)i*16 + (uint16_t)j]);
				HAL_UART_Transmit(&huart1,(uint8_t*)str1,1,0x1000);
			}
			HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0x1000);
		}
		HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0x1000);
	}

}
//---------------------------------------------------------
void W25_Info(void)
{
	uint32_t Jedec_ID = W25_Read_Jedec_ID();
	Jedec_ID &= 0x0000ffff;
	char str[30];
	switch(Jedec_ID)
	{
	case 0x401A:
		w25_info.BlockCount=1024;
		sprintf(str,"w25qxx Chip: w25q512\r\n");
		break;
	case 0x4019:
		w25_info.BlockCount=512;
		sprintf(str,"w25qxx Chip: w25q256\r\n");
		break;
	case 0x4018:
		w25_info.BlockCount=256;
		sprintf(str,"w25qxx Chip: w25q128\r\n");
		break;
	case 0x4017:
		w25_info.BlockCount=128;
		sprintf(str,"w25qxx Chip: w25q64\r\n");
		break;
	case 0x4016:
		w25_info.BlockCount=64;
		sprintf(str,"w25qxx Chip: w25q32\r\n");
		break;
	case 0x4015:
		w25_info.BlockCount=32;
		sprintf(str,"w25qxx Chip: w25q16\r\n");
		break;
	case 0x4014:
		w25_info.BlockCount=16;
		sprintf(str,"w25qxx Chip: w25q80\r\n");
		break;
	case 0x4013:
		w25_info.BlockCount=8;
		sprintf(str,"w25qxx Chip: w25q40\r\n");
		break;
	case 0x4012:
		w25_info.BlockCount=4;
		sprintf(str,"w25qxx Chip: w25q20\r\n");
		break;
	case 0x4011:
		w25_info.BlockCount=2;
		sprintf(str,"w25qxx Chip: w25q10\r\n");
		break;
	default:
		sprintf(str,"w25qxx Unknown ID\r\n");
		break;
	}
	HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
	w25_info.PageSize=256;
	w25_info.SectorSize=0x1000;
	w25_info.SectorCount=w25_info.BlockCount*16;
	w25_info.PageCount=(w25_info.SectorCount*w25_info.SectorSize)/w25_info.PageSize;
	w25_info.BlockSize=w25_info.SectorSize*16;
	w25_info.NumKB=(w25_info.SectorCount*w25_info.SectorSize)/1024;
	sprintf(str,"Page Size: %d Bytes\r\n",(unsigned int)w25_info.PageSize);
	HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
	sprintf(str,"Page Count: %u\r\n",(unsigned int)w25_info.PageCount);
	HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
	sprintf(str,"Sector Size: %u Bytes\r\n",(unsigned int)w25_info.SectorSize);
	HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
	sprintf(str,"Sector Count: %u\r\n",(unsigned int)w25_info.SectorCount);
	HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
	sprintf(str,"Block Size: %u Bytes\r\n",(unsigned int)w25_info.BlockSize);
	HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
	sprintf(str,"Block Count: %u\r\n",(unsigned int)w25_info.BlockCount);
	HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
	sprintf(str,"Capacity: %u KB\r\n",(unsigned int)w25_info.NumKB);
	HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
}
//---------------------------------------------------------
uint32_t W25_Read_Jedec_ID(void)
{
	uint8_t dt[4];
	buf[0] = W25_GET_JEDEC_ID;
	cs_set();
	SPI1_Send(buf, 1);
	SPI1_Recv(dt,3);
	cs_reset();
	return (dt[0] << 16) | (dt[1] << 8) | dt[2];
}
//---------------------------------------------------------
void W25_Init(void)
{
	HAL_Delay(100);
	W25_Reset();
	HAL_Delay(100);
}
//---------------------------------------------------------