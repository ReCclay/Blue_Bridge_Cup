/*
*******************************************************************************
* 文件名：lcd1602.h
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#ifndef LCD1602_H
#define LCD1602_H

void InitLcd1602();
void LcdFullClear(u8 x, u8 y, u8 len);
void LcdAreaClear();
void LcdShowStr(u8 x, u8 y, u8 *str);


#endif