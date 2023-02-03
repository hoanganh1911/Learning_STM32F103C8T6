/*
 * MC3419.h
 *
 *  Created on: Feb 3, 2023
 *      Author: hoanganh
 */

#ifndef MC3419_H_
#define MC3419_H_
//---------------------------------------------------------
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "i2c.h"
//---------------------------------------------------------
extern I2C_HandleTypeDef *hi2c;
#define MC3419_ADDR 0x4C << 1
//---------------------------------------------------------
#define MC34X9_REG_MODE             (0x07)
#define MC34X9_REG_SR               (0x08)
#define MC34X9_REG_MOTION_CTRL      (0x09)
#define MC34X9_REG_XOUT_LSB         (0x0D)
#define MC34X9_REG_XOUT_MSB         (0x0E)
#define MC34X9_REG_YOUT_LSB         (0x0F)
#define MC34X9_REG_YOUT_MSB         (0x10)
#define MC34X9_REG_ZOUT_LSB         (0x11)
#define MC34X9_REG_ZOUT_MSB         (0x12)
#define MC34X9_REG_RANGE_C          (0x20)
typedef struct
{
	float x;
	float y;
	float z;
}_ACCRAW;
typedef enum
{
	MC34X9_MODE_STANDBY  	= 0b000,
	MC34X9_MODE_CWAKE      	= 0b001,

}   MC34X9_mode_t;
typedef enum
{
  MC34X9_RANGE_2G    = 0b000,
  MC34X9_RANGE_4G    = 0b001,
  MC34X9_RANGE_8G    = 0b010,
  MC34X9_RANGE_16G   = 0b011,
  MC34X9_RANGE_12G   = 0b100,
  MC34X9_RANGE_END,
}   MC34X9_range_t;
typedef enum //sample rate
{
  MC34X9_SR_25Hz            = 0x10,
  MC34X9_SR_50Hz            = 0x11,
  MC34X9_SR_62_5Hz          = 0x12,
  MC34X9_SR_100Hz           = 0x13,
  MC34X9_SR_125Hz           = 0x14,
  MC34X9_SR_250Hz           = 0x15,
  MC34X9_SR_500Hz           = 0x16,
  MC34X9_SR_DEFAULT_1000Hz  = 0x17,
  MC34X9_SR_END,
}   MC34X9_sr_t;
void readRawAccel(_ACCRAW *accraw);
void sensorMotion(void);
void start(void);
#endif /* MC3419_H_ */
