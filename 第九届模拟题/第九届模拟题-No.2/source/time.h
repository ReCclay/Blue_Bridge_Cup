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

void KeyAction(u8 keycode);
void RefreshTime(bit sta);
void RefreshAlarm();
void Refresh18B20();
void AlarmMonitor();

#endif