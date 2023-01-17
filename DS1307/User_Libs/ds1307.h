#ifndef _DS1307_H_
#define _DS1307_H_
//---------------------------------------------------------
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "i2c.h"
extern I2C_HandleTypeDef *hi2c;
//---------------------------------------------------------
#define DS1307_REG_TIME	0x00
typedef enum
{
  SUNDAY = 1,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY
} DayOfWeek;
typedef struct
{
	uint8_t Year;
	uint8_t Month;
	uint8_t Date;
	uint8_t DayOfWeek;
	uint8_t Hour;
	uint8_t Min;
	uint8_t Sec;
} _RTC;
void ds1307_Init(I2C_HandleTypeDef *handle);
bool ds1307_getTime(_RTC *rtc);
bool ds1307_setTime(_RTC *rtc);
#endif /* __DS1307_H_ */
