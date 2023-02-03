
#include "lcd.h"
extern I2C_HandleTypeDef *hi2c;
#define LCD_ADDR 0x20 << 1

bool lcd_Send_Cmd(char cmd) //Mỗi lần gửi sẽ gửi 4 bit cao xong rồi đến 4 bit thấp
{
	char data_u, data_l; // vi du 0x30
	uint8_t data_t[4];
	data_u = (cmd & 0xf0); // data_u = 	0x30
	data_l = ((cmd << 4) & 0xf0); // data_l = 0x00
//	data_t[0] = data_u | 0x0C; // 0000 1100 | 0011 0000 = 0011 1100 = 0x3C
//	data_t[1] = data_u | 0x08; // 0000 1000 | 0011 0000 = 0011 1000 = 0x38
//	data_t[2] = data_l | 0x0C; // 0000 1100 | 0000 0000 = 0000 1100 = 0x0C
//	data_t[3] = data_l | 0x08; // 0000 1000 | 0000 0000 = 0000 1000 = 0x08
	//dịch bit với mạch joystick robocon
	data_t[0] = (data_u >> 1) | 0x04 | 0x80; // 1000 0000 | 0000 0100 | 0001 1000 = 1001 1100
	data_t[1] = (data_u >> 1) | 0x00 | 0x80; // 1000 0000 | 0000 0000 | 0001 1000 = 1001 1000
	data_t[2] = (data_l >> 1) | 0x04 | 0x80;
	data_t[3] = (data_l >> 1) | 0x00 | 0x80;
	if(HAL_I2C_Master_Transmit(hi2c, LCD_ADDR,(uint8_t *)data_t, 4, 100) != HAL_OK) return false;
	return true;
}
bool lcd_Send_Data(char data)
{
	char data_u,data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
//	data_t[0] = data_u | 0x0D; // 1101
//	data_t[1] = data_u | 0x09; // 1001
//	data_t[2] = data_l | 0x0D;
//	data_t[3] = data_l | 0x09;
	//dịch bit với mạch joystick robocon
	data_t[0] = (data_u >> 1)  | 0x05 | 0x80;
	data_t[1] = (data_u >> 1)  | 0x01 | 0x80;
	data_t[2] = (data_l >> 1)  | 0x05 | 0x80;
	data_t[3] = (data_l >> 1)  | 0x01 | 0x80;
	if(HAL_I2C_Master_Transmit(hi2c, LCD_ADDR, (uint8_t *)data_t, 4, 100) != HAL_OK) return false;
	return true;
}
void lcd_Clear(void)
{
	lcd_Send_Cmd(0x80);
	for(int i = 0; i < 100; i++)
	{
		lcd_Send_Data(' ');
	}
}
void lcd_Put_Cur(int row,int col)
{
	switch(row)
	{
		case 0:
			col |= 0x80; //0x80 1 000 000x
			break;
		case 1:
			col |= 0xC0; // 0xC1 1 100 000x
			break;
		case 2:
			//col |= 0x90;
			col = (col+3) | 0x90;
			break;
		case 3:
			//col |= 0xD0;
			col = (col+3) | 0xD0;
			break;
	}
	//Gửi lệnh ứng với vị trí cần xuất hiện con trỏ Set DDRAM address tr24
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
	// Khởi tạo hiển thị
	//00 00 101 00
	lcd_Send_Cmd(0x28); //function set bảng 6/tr23: set data interface 4bits 2lines 5x8 bit
	HAL_Delay(1);
	lcd_Send_Cmd(0x08); //display on/off control
	HAL_Delay(1);
	lcd_Send_Cmd(0x01); //clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_Send_Cmd(0x06); //entry mode set: i/d=1 increment s=0
	HAL_Delay(1);
	lcd_Send_Cmd(0x0C); // cho phep hien thi man hinh
}
void lcd_Send_String(char *str)
{
	while(*str) lcd_Send_Data(*str++);
}
