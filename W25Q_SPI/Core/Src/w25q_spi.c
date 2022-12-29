#include "w25q_spi.h"
SPI_HandleTypeDef hspi1;
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
void W25_Ini(void)
{
	HAL_Delay(100);
	W25_Reset();
	HAL_Delay(100);
}
//---------------------------------------------------------
