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

enum estaSys{
	E_SELECT, E_CLK, E_DISPLAY
};

extern bit TurnSta;
extern enum estaSys staSys;
extern int temp123;

#endif	 