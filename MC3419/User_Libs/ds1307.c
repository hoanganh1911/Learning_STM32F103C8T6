#include "ds1307.h"
#define DS1307_ADDR 0x68 << 1

static uint8_t B2D(uint8_t bcd);
static uint8_t D2B(uint8_t decimal);

bool ds1307_getTime(_RTC *rtc)
{
	uint8_t startAddr = DS1307_REG_TIME;
	uint8_t buffer[7] = {0,};
	if(HAL_I2C_Master_Transmit(hi2c,DS1307_ADDR, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK) return false;
	if(HAL_I2C_Master_Receive(hi2c, DS1307_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK) return false;
	rtc->Sec = B2D(buffer[0] & 0x7F);
	rtc->Min = B2D(buffer[1] & 0x7F);
	rtc->Hour = B2D(buffer[2] & 0x3F);
	rtc->DayOfWeek = buffer[3] & 0x07;
	rtc->Date = B2D(buffer[4] & 0x3F);
	rtc->Month = B2D(buffer[5] & 0x1F);
	rtc->Year = B2D(buffer[6]);

	return true;
}

bool ds1307_setTime(_RTC *rtc)
{
	uint8_t startAddr = DS1307_REG_TIME;
	uint8_t buffer[8] = {startAddr,D2B(rtc->Sec),D2B(rtc->Min),D2B(rtc->Hour),rtc->DayOfWeek,D2B(rtc->Date),D2B(rtc->Month),D2B(rtc->Year)};
	if(HAL_I2C_Master_Transmit(hi2c, DS1307_ADDR, buffer, sizeof(buffer),HAL_MAX_DELAY) != HAL_OK) return false;

	return true;
}

static uint8_t B2D(uint8_t bcd) //hàm để chuyển giá trị ở các bit hàng chục
{
  return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static uint8_t D2B(uint8_t decimal) //hàm để chuyển giá trị ở các bit hàng chục
{
  return (((decimal / 10) << 4) | (decimal % 10));
}
