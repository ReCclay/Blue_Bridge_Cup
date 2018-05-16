/*
*******************************************************************************
* 文件名：time.h
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#ifndef TIME_H
#define TIME_H

extern u8 SetSec;
extern bit flagLed;
extern bit flagtempOver;

void RefreshTime(bit sta);
void KeyAction(u8 keycode);
void RefreshTemp();
void RefreshSetSec();
void EnterTemp(bit sta);
void CollectTemp();

#endif