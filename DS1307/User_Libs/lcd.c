/*
 * lcd.c
 *
 *  Created on: Jan 17, 2023
 *      Author: hoanganh
 */
#include "lcd.h"
extern I2C_HandleTypeDef *hi2c;
#define LCD_ADDR 0x27 << 1

bool lcd_Send_Cmd(char cmd)
{
	char data_u, data_l; // vi du 0x30
	uint8_t data_t[4];
	data_u = (cmd & 0xf0); // data_u = 	0x30
	data_l = ((cmd << 4) & 0xf0); // data_l = 0x00
	data_t[0] = data_u | 0x0C; // 0000 1100 | 0011 0000 = 0011 1100 = 0x3C
	data_t[1] = data_u | 0x08; // 0000 1000 | 0011 0000 = 0011 1000 = 0x38
	data_t[2] = data_l | 0x0C; // 0000 1100 | 0000 0000 = 0000 1100 = 0x0C
	data_t[3] = data_l | 0x08; // 0000 1000 | 0000 0000 = 0000 1000 = 0x08
	if(HAL_I2C_Master_Transmit(hi2c, LCD_ADDR,(uint8_t *)data_t, 4, 100) != HAL_OK) return false;
	return true;
}
bool lcd_Send_Data(char data)
{
	char data_u,data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = data_u | 0x0D; // 1101
	data_t[1] = data_u | 0x09; // 1001
	data_t[2] = data_l | 0x0D;
	data_t[3] = data_l | 0x09;
	if(HAL_I2C_Master_Transmit(hi2c, LCD_ADDR, (uint8_t *)data_t, 4, 100) != HAL_OK) return false;
	return true;
}
void lcd_Clear(void)
{
	lcd_Send_Cmd(0x80);
	for(int i = 0; i < 70; i++)
	{
		lcd_Send_Data(' ');
	}
}
void lcd_Put_Cur(int row,int col)
{
	switch(row)
	{
		case 0:
			col |= 0x80;
			break;
		case 1:
			row |= 0xC0;
			break;
	}
	lcd_Send_Cmd(col);
}
void lcd_Init(void)
{
	// Khởi tạo 4 bit
	HAL_Delay(50);
	lcd_Send_Cmd(0x30);
	HAL_Delay(5);
	lcd_Send_Cmd(0x30);
	HAL_Delay(1);
	lcd_Send_Cmd(0x30);
	HAL_Delay(10);
	lcd_Send_Cmd(0x20);
	HAL_Delay(10);

	lcd_Send_Cmd(0x28);
	HAL_Delay(1);
	lcd_Send_Cmd(0x08);
	HAL_Delay(1);
	lcd_Send_Cmd(0x01);
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_Send_Cmd(0x06);
	HAL_Delay(1);
	lcd_Send_Cmd(0x0C);
}
void lcd_Send_String(char *str)
{
	while(*str) lcd_Send_Data(*str++);
}



