/*
*******************************************************************************
* 文件名：ds1302.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#include "config.h"
#include <intrins.h>
#include "ds1302.h"

void DS1302ByteWrite(u8 temp) 
{
	u8 i;
	for (i=0;i<8;i++)     	
	{ 
		DS1302_CK=0;
		DS1302_IO=temp&0x01;
		temp>>=1; 
		DS1302_CK=1;
	}
}  

u8 DS1302ByteRead()
{
 	u8 i, temp=0x00;

	for (i=0;i<8;i++) 	
 	{		
		DS1302_CK=0;
		temp>>=1;	
 		if(DS1302_IO)
 		temp|=0x80;	
 		DS1302_CK=1;
	} 

	return temp;
} 

void DS1302SingleWrite(u8 address,u8 dat )     
{
 	DS1302_CE=0;
	_nop_();
 	DS1302_CK=0;
	_nop_();
 	DS1302_CE=1;	
   	_nop_();  
 	DS1302ByteWrite((address<<1)|0x80);	
 	DS1302ByteWrite(dat);		
 	DS1302_CE=0; 
}

u8 DS1302SingleRead (u8 address )
{
 	u8 temp=0x00;

 	DS1302_CE=0;
	_nop_();
 	DS1302_CK=0;
	_nop_();
 	DS1302_CE=1;
	_nop_();
 	DS1302ByteWrite((address<<1)|0x81);
	temp = DS1302ByteRead();
 	DS1302_CE=0;
	_nop_();
 	DS1302_CE=0;
	DS1302_CK=0;
	_nop_();
	DS1302_CK=1;
	_nop_();
	DS1302_IO=0;
	_nop_();
	DS1302_IO=1;
	_nop_();

	return temp;			
}

void DS1302BurstWrite(u8 *dat)
{
	u8 i;

 	DS1302_CE=0;
	_nop_();
 	DS1302_CK=0;
	_nop_();
 	DS1302_CE=1;	
   	_nop_();  
 	DS1302ByteWrite(0xBE);
	for(i=0; i<8; i++)
	{	
 		DS1302ByteWrite(*dat++);					   
	}
 	DS1302_CE=0; 	
}

void DS1302BurstRead (u8 *dat)
{
  	u8 i;

 	DS1302_CE=0;
	_nop_();
 	DS1302_CK=0;
	_nop_();
 	DS1302_CE=1;
	_nop_();
 	DS1302ByteWrite(0xBF);
	for(i=0; i<8; i++)
	{
		dat[i] = DS1302ByteRead();
	}
 	DS1302_CE=0;
	_nop_();
 	DS1302_CE=0;
	DS1302_CK=0;
	_nop_();
	DS1302_CK=1;
	_nop_();
	DS1302_IO=0;
	_nop_();
	DS1302_IO=1;
	_nop_();		
}

void GetRealTime(struct sTime *time)
{
	u8 buf[8];

	DS1302BurstRead(buf);
	time->year = buf[6] + 0x2000;
	time->mon  = buf[4];
	time->day  = buf[3];
	time->hour = buf[2];
	time->min  = buf[1];
	time->sec  = buf[0];
	time->week = buf[5];
}

void SetRealTime(struct sTime *time)
{
 	u8 buf[8];

	buf[7] = 0;
	buf[6] = time->year;
	buf[4] = time->mon;
	buf[3] = time->day;
	buf[2] = time->hour;
	buf[1] = time->min;
	buf[0] = time->sec;
	buf[5] = time->week;
	DS1302BurstWrite(buf);
}

void InitDS1302()
{
    struct sTime InitTime[] = {  //2018年3月3日 星期六 23:59:50
        0x18, 0x03, 0x04, 0x23, 0x59, 0x50, 0x07
    };
    DS1302SingleWrite(7, 0x00);  //撤销写保护以允许写入数据
	SetRealTime(&InitTime);
}