/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：来看下封装成结构体如何实现电子钟！
*         S8-ESC键，S12-回车键，S19-增加，S18-减小，S17-左移，S16-右移
*******************************************************************************
*/

#include "config.h"
#include "ds1302.h"
#include "keyboard.h"
#include "led.h"
#include "time.h"

u8 T0RH = 0, T0RL = 0;
bit flag200ms = 1;
bit flag1s = 0;
bit sta = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();
void RefreshTimeShow();

void main()
{
	u8 psec = 0xAA;
	struct sTime buffTime;

	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();

	while(1)
	{
		KeyDriver();
		if((flag200ms !=0) && (setIndex == 0)) //200ms定时到，且未处于设置模式
		{
		 	flag200ms = 0;
			GetRealTime(&buffTime);
			if(psec != buffTime.sec)
			{
			 	RefreshTime(1);
				psec = buffTime.sec;
			}
		}	
		if(flagCursor)
		{
		 	if(flag1s)
			{
			 	flag1s = 0;
				if(sta == 1)
				{
				 	sta = 0;
					LedBuff[setCursorIndex] = 0xFF;
					LedBuff[setCursorIndex-1] = 0xFF;
				}
				else
				{
				 	sta = 1;
					RefreshTime(0);
				}
			}
		}
	}
}

void CloseFucker()
{
 	P2 = (P2 & 0x1F) | 0xA0;
	P0 = P0 & 0xAF;
	P2 = P2&0x1F;
}

void ConfigTimer0(u16 ms)
{
    u32 tmp; 
    
    tmp = 11059200 / 12;    
    tmp = (tmp * ms) / 1000;  
    tmp = 65536 - tmp;          
    T0RH = (u8)(tmp>>8);  
    T0RL = (u8)tmp;
    TMOD &= 0xF0;   
    TMOD |= 0x01;   
    TH0 = T0RH;     
    TL0 = T0RL;
    ET0 = 1;       
    TR0 = 1;       
}

void InterruptTimer0() interrupt 1
{
    static u8 tmr200ms = 0;
	static u16 tmr1s = 0;
    
    TH0 = T0RH; 
    TL0 = T0RL;
	KeyScan();
	LedScan();

    tmr200ms++;
	tmr1s++;

    if(tmr200ms >= 200) 
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
	if(tmr1s >= 1000)
	{
	 	tmr1s = 0;
		flag1s = 1;
	}
}
