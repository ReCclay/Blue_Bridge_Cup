/*
*******************************************************************************
* 文件名：led.h
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#ifndef LED_H
#define LED_H

extern u8 code LedChar[];
extern u8 LedBuff[];
extern bit flagCursor;
extern u8 setCursorIndex;

void ShowNumber(u32 dat);
void LedScan();
void LedOpenCursor();
void LedCloseCursor();
void LedSetCursor(u8 index);

#endif