/*
 * lcd.c
 *
 *  Created on: Jan 17, 2023
 *      Author: hoanganh
 */
#include "lcd.h"

#define LCD_ADDR 0x27 << 1

void lcd_Send_Cmd(char cmd)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = data_u | 0x0C;
	data_t[1] = data_u | 0x08;
	data_t[2] = data_l | 0x0C;
	data_t[3] = data_l | 0x08;
	HAL_I2C_Master_Transmit(hi2c, LCD_ADDR,(uint8_t *)data_t, 4, 100);
}
void lcd_Send_Data(char data)
{
	char data_u,data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = data_u | 0x0D;
	data_t[1] = data_u | 0x09;
	data_t[2] = data_l | 0x0D;
	data_t[3] = data_l | 0x09;
	HAL_I2C_Master_Transmit(hi2c, LCD_ADDR, (uint8_t *)data_t, 4, 100);
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

}




