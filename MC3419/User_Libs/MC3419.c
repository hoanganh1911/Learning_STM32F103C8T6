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
	uint16_t startAddr = MC34X9_REG_XOUT_LSB;
	float faRange[5] = { 19.614f, 39.228f, 78.456f, 156.912f, 117.684f};
	// 16bit
	float faResolution = 32768.0f;
	uint16_t *buffer;
	buffer = (uint8_t *)malloc(6);
	HAL_I2C_Mem_Read(hi2c, MC3419_ADDR, startAddr, 1, buffer, 6, HAL_MAX_DELAY);
	accraw->x = (buffer[1] << 8 | buffer[0])/ faResolution * faRange[CfgRange];
	accraw->y = (buffer[3] << 8 | buffer[2])/ faResolution * faRange[CfgRange];
	accraw->z = (buffer[5] << 8 | buffer[4])/ faResolution * faRange[CfgRange];
	free(buffer);
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
  uint8_t value;
  CfgRange = range;
  SetMode(MC34X9_MODE_STANDBY);
  HAL_I2C_Mem_Read(hi2c, MC3419_ADDR, MC34X9_REG_RANGE_C, 1, &value, 1, HAL_MAX_DELAY);
  value &= 0b00000111;
  value |= (range << 4) & 0x70;
  HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, MC34X9_REG_RANGE_C, 1, &value, 1, HAL_MAX_DELAY);
}
void SetSampleRate(MC34X9_sr_t sample_rate)
{
	uint8_t startAddr = MC34X9_REG_SR;
	uint8_t value;
	SetMode(MC34X9_MODE_STANDBY);
	HAL_I2C_Mem_Read(hi2c, MC3419_ADDR, startAddr, 1, &value, 1, HAL_MAX_DELAY);
	value &= 0b00000000;
	value |= sample_rate;
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, startAddr, 1, &value, 1, HAL_MAX_DELAY);
}

void SetMotionCtrl() // Chi set lac do dang test
{
	uint8_t startAddr = MC34X9_REG_MOTION_CTRL;
	uint8_t CfgMotion = 0b00001100;
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, startAddr, 1, &CfgMotion, 1, HAL_MAX_DELAY);
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
	// power-on-reset
	temp = 0x40;
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, 0x1C, 1, &temp, 1, HAL_MAX_DELAY);
	HAL_Delay(50);
	//Disabe interrupt
	temp = 0x00;
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, 0x06, 1, &temp, 1, HAL_MAX_DELAY);
	HAL_Delay(10);
	// 1.00x Aanalog Gain
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, 0x2B, 1, &temp, 1, HAL_MAX_DELAY);
	HAL_Delay(10);
	// DCM disable
	HAL_I2C_Mem_Write(hi2c, MC3419_ADDR, 0x15, 1, &temp, 1, HAL_MAX_DELAY);
	HAL_Delay(50);
}
//Initialize the MC34X9 sensor and set as the default configuration
void start() // hiện tại là i2c
{
	//Init reset
	reset();
	SetMode(MC34X9_MODE_STANDBY);
	//Range: 8g
	SetRangeCtrl(MC34X9_CFG_RANGE_DEFAULT);
	//Sampling Rate: 50Hz by default
	SetSampleRate(MC34X9_CFG_SAMPLE_RATE_DEFAULT);
	//Mode: Active
	SetMode(MC34X9_MODE_CWAKE);
	HAL_Delay(50);
}


