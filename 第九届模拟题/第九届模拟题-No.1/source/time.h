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

extern bit flagAlarm;
extern bit flagCursorCnt;

void KeyAction(u8 keycode);
void RefreshTime(bit ak);
void RefreshAlarm();
void AlarmMonitor();
void Refresh18B20();


#endif