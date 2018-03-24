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

void LedScan();
void ShowNumber(u32 dat);

#endif