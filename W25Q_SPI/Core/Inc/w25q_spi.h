#ifndef _W25Q_SPI_H
#define _W25Q_SPI_H
//---------------------------------------------------------
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//---------------------------------------------------------
#define W25_ENABLE_RESET 0x66
#define W25_RESET 0x99
#define W25_READ 0x03
#define W25_FAST_READ 0x0B
#define W25_GET_JEDEC_ID 0x9f
#define W25_UNIQUE_ID 0x4B
#define W25_WRITE_DISABLE  0x04
#define W25_WRITE_ENABLE 0x06
#define W25_SECTOR_ERASE 0x20
#define W25_BLOCK_ERASE  0xD8
#define W25_CHIP_ERASE 0xC7
#define W25_READ_STATUS_1  0x05
#define W25_READ_STATUS_2  0x35
#define W25_READ_STATUS_3  0x15
#define W25_WRITE_STATUS_1 0x01
#define W25_WRITE_STATUS_2 0x31
#define W25_WRITE_STATUS_3 0x11
#define W25_PAGE_PROGRAMM  0x02
//---------------------------------------------------------
#define cs_set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)
#define cs_reset() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)
//---------------------------------------------------------
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;
//---------------------------------------------------------
typedef struct
{
  uint16_t  PageSize;
  uint32_t  PageCount;
  uint32_t  SectorSize;
  uint32_t  SectorCount;
  uint32_t  BlockSize;
  uint32_t  BlockCount;
  uint32_t  NumKB;
  uint8_t   SR1;
  uint8_t   SR2;
  uint8_t   SR3;
  uint8_t	high_cap;
  uint8_t   StatusRegister1;
  uint8_t   StatusRegister2;
  uint8_t   StatusRegister3;
}w25_info_t;

//---------------------------------------------------------
void SPI1_Send(uint8_t *dt,uint16_t cnt);
void SPI1_Recv(uint8_t *dt,uint16_t cnt);
void W25_Reset(void);
void W25_Read_Data(uint32_t addr,uint8_t* data,uint32_t sz);
uint64_t W25_Read_ID(void);
uint32_t W25_Read_Jedec_ID(void);
void W25_Init(void);
void W25_Info(void);
void UART_64bit(uint64_t x);
void W25_PrintUart_ReadData(void);
void W25_Write_Enable(void);
void W25_Erase_Sector(uint32_t addr);
void W25_Erase_Block(uint32_t addr);
void W25_Erase_Chip(void);
void W25_Write_Data(uint32_t addr, uint8_t* data, uint32_t sz);
//---------------------------------------------------------
#endif /* __W25Q_SPI_H */
