/*
 * lcd.h
 *
 *  Created on: Jan 17, 2023
 *      Author: hoanganh
 */

#ifndef LCD_H_
#define LCD_H_
//---------------------------------------------------------
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "i2c.h"

//---------------------------------------------------------
bool lcd_Send_Cmd(char cmd);
bool lcd_Send_Data(char data);
void lcd_Clear(void);
void lcd_Put_Cur(int row,int col);
void lcd_Init(void);
void lcd_Send_String(char *str);
#endif /* LCD_H_ */
