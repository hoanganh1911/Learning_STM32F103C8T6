/*
 * i2c.c
 *
 *  Created on: Jan 17, 2023
 *      Author: hoanganh
 */
#include "i2c.h"

I2C_HandleTypeDef *hi2c;
void lcd_Init(I2C_HandleTypeDef *handle)
{
	hi2c = handle;
}


