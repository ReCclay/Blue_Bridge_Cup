/*
*******************************************************************************
* 文件名：main.h
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#ifndef MAIN_H
#define MAIN_H

enum eSystemSta{//分别对应正常运行状态，设置时间状态，设置闹钟状态
	E_NORMAL, E_SET_TIME, E_SET_ALARM
};

extern enum eSystemSta SystemSta; 

#endif