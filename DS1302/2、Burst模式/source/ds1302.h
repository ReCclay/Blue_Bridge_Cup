/*
*******************************************************************************
* 文件名：ds1302.h
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#ifndef DS1302_H
#define DS1302_H

u8 DS1302SingleRead(u8 reg);
void  DS1302SingleWrite(u8 reg, u8 dat);
void InitDS1302();
void DS1302BurstWrite(u8 *dat);
void DS1302BurstRead(u8 *dat);


#endif