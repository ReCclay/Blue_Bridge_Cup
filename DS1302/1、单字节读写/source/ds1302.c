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
#include "ds1302.h"

void DS1302ByteWrite(u8 dat) 
{
	u8 mask;
    
    for (mask=0x01; mask!=0; mask<<=1)  //低位在前，逐位移出
    {
        if ((mask&dat) != 0) //首先输出该位数据
            DS1302_IO = 1;
        else
            DS1302_IO = 0;
        DS1302_CK = 1;       //然后拉高时钟
        DS1302_CK = 0;       //再拉低时钟，完成一个位的操作
    }
    DS1302_IO = 1;           //最后确保释放IO引脚
}  

u8 DS1302ByteRead()
{
	u8 mask;
    u8 dat = 0;
    
    for (mask=0x01; mask!=0; mask<<=1)  //低位在前，逐位读取
    {
        if (DS1302_IO != 0)  //首先读取此时的IO引脚，并设置dat中的对应位
        {
            dat |= mask;
        }
        DS1302_CK = 1;       //然后拉高时钟
        DS1302_CK = 0;       //再拉低时钟，完成一个位的操作
    }
    return dat;              //最后返回读到的字节数据
} 

void DS1302SingleWrite(u8 reg,u8 dat )     
{
 	DS1302_CE = 1;	 
 	DS1302ByteWrite((reg<<1)|0x80);	
 	DS1302ByteWrite(dat);		
 	DS1302_CE = 0; 
}

u8 DS1302SingleRead (u8 reg)
{
 	u8 dat=0;

 	DS1302_CE = 1;
 	DS1302ByteWrite((reg<<1)|0x81);
	dat = DS1302ByteRead();
 	DS1302_CE = 0;
	DS1302_IO = 0;//单字节读完之后必须加这一句！玄学？？？

	return dat;			
}

void InitDS1302()
{
	u8 i;
    u8 code InitTime[] = {  //2018年3月17日 星期六 11:12:00
        0x00, 0x12, 0x11, 0x17, 0x03, 0x06, 0x18
    };

    DS1302_CE = 0;  //初始化DS1302通信引脚
    DS1302_CK = 0;
    DS1302SingleWrite(7, 0x00);  //撤销写保护以允许写入数据
	for (i=0; i<7; i++)          //设置DS1302为默认的初始时间
    {
        DS1302SingleWrite(i, InitTime[i]);
    }
}