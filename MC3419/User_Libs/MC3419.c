/*
 * MC3419.c
 *
 *  Created on: Feb 3, 2023
 *      Author: hoanganh
 */
#include "MC3419.h"
#define MC34X9_CFG_MODE_DEFAULT                 MC34X9_MODE_STANDBY
#define MC34X9_CFG_SAMPLE_RATE_DEFAULT    		MC34X9_SR_DEFAULT_1000Hz
#define MC34X9_CFG_RANGE_DEFAULT                MC34X9_RANGE_8G
uint8_t CfgRange, CfgFifo;
void readRawAccel(_ACCRAW *accraw)
{
	float faRange[5] = { 19.614f, 39.228f, 78.456f, 156.912f, 117.684f};
	// 16bit
	float faResolution = 32768.0f;
	uint8_t buffer[6];

	HAL_I2C_Mem_Read(hi2c, MC3419_ADDR, MC34X9_REG_XOUT_LSB, 1, &buffer[0], 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c, MC3419_ADDR, MC34X9_REG_XOUT_MSB, 1, &buffer[1], 1, HAL_MAX_DELAY);
	accraw->x = (float)((short)((unsigned short)buffer[1] << 8 | buffer[0])) / faResolution * faRange[CfgRange];

	HAL_I2C_Mem_Read(hi2c, MC3419_ADDR, MC34X9_REG_YOUT_LSB, 1, &buffer[2], 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c, MC3419_ADDR, MC34X9_REG_YOUT_MSB, 1, &buffer[3], 1, HAL_MAX_DELAY);
	accraw->y = (float)((short)((unsigned short)buffer[3] << 8 | buffer[2])) / faResolution * faRange[CfgRange];

	HAL_I2C_Mem_Read(hi2c, MC3419_ADDR, MC34X9_REG_ZOUT_LSB, 1, &buffer[4], 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c, MC3419_ADDR, MC34X9_REG_ZOUT_MSB, 1, &buffer[5], 1, HAL_MAX_DELAY);
	accraw->z = (float)((short)((unsigned short)buffer[5] << 8 | buffer[4])) / faResolution * faRange[CfgRange];
}
void SetMode(MC34X9_mode_t mode)
{
	uint16_t startAddr = MC34X9_REG_MODE;
	uint8_t value = 0;
	value |= mode;
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, startAddr, 1, &value, 1, HAL_MAX_DELAY);
}
void stop()
{
	SetMode(MC34X9_MODE_STANDBY);
}

void wake()
{
	SetMode(MC34X9_MODE_CWAKE);
}

void SetRangeCtrl(MC34X9_range_t range)
{
  uint8_t value = 0;
  CfgRange = range;
  value |= 0x08 | range;
  HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, MC34X9_REG_RANGE_C, 1, &value, 1, HAL_MAX_DELAY);
}
void SetSampleRate(MC34X9_sr_t sample_rate)
{
	uint8_t value = 0;
	value |= sample_rate;
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, MC34X9_REG_SR, 1, &value, 1, HAL_MAX_DELAY);
}

void SetMotionCtrl() // Chi set lac do dang test
{
	uint8_t CfgMotion = 0b00100000;
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, MC34X9_REG_MOTION_CTRL, 1, &CfgMotion, 1, HAL_MAX_DELAY);
}
void sensorMotion()
{
	stop();
	SetSampleRate(MC34X9_SR_DEFAULT_1000Hz);
	SetMotionCtrl();
	wake();
}
void reset()
{
	// Stand by mode
	SetMode(MC34X9_MODE_STANDBY);
	HAL_Delay(10);
	uint8_t temp;
	//Disabe interrupt
	temp = 0x00;
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, 0x06, 1, &temp, 1, HAL_MAX_DELAY);
	HAL_Delay(10);
}
//Initialize the MC34X9 sensor and set as the default configuration
void start() // hiện tại là i2c
{
	//Init reset
	reset();
	SetMode(MC34X9_MODE_STANDBY);
	//Range: 8g
	SetRangeCtrl(MC34X9_CFG_RANGE_DEFAULT);
	//Sampling Rate: 1000Hz by default
	SetSampleRate(MC34X9_CFG_SAMPLE_RATE_DEFAULT);
	//Mode: Active
	SetMode(MC34X9_MODE_CWAKE);
	HAL_Delay(50);
}


