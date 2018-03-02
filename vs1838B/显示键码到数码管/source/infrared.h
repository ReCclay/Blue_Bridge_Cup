/*
*******************************************************************************
* 文件名：infrared.h
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#ifndef INFRARED_H
#define INFRARED_H

void InitInfrared();
void IRScan();

extern bit irflag;
extern u8 ircode[4];

#endif