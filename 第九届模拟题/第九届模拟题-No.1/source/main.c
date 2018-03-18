/*
*******************************************************************************
* 文件名：main.c
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
#include "time.h"
#include "keyboard.h"
#include "main.h"
#include "led.h"
#include "ds18b20.h"

u8 T0RH = 0, T0RL = 0;
bit flag200ms = 1;
bit flag1s = 0;

enum eStaSystem staSystem = E_NORMAL;

void ConfigTimer0(u16 ms);
void CloseFucker();

void main()
{
	bit sta = 0; //光标亮灭对应的标志

	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();
	
	while(1)
	{
		KeyDriver();
		if(flag200ms) //200ms定时到
		{
			flag200ms = 0;
			AlarmMonitor();
			if((staSystem == E_NORMAL) && (!flag18B20))//当前系统的模式是时钟显示，且没有按下S4 - 刷新当前时间
			{
				RefreshTime(1);	
			}
			else if((staSystem == E_NORMAL) && (flag18B20))//当前系统模式处于时钟显示，且按下s4 - 进入温度显示
			{
			 	Refresh18B20();
			}

		}
		if(flagCursor)//在设置模式，实现光标闪烁的效果（数码管闪烁）
		{
		 	if(flag1s)//1s的间隔亮灭
			{
			 	flag1s = 0;
				if(sta == 1)//当前是亮的状态
				{
					sta = 0;
					LedBuff[setCursorIndex] = 0xFF;//关闭数码管
					LedBuff[setCursorIndex-1] = 0xFF;
				}
				else //当前是灭的状态
				{
					sta = 1;
					if(staSystem == E_SET_TIME)//！！！这一点需要判断一下当前是什么设置模式是刷新时钟还是闹钟！
					{
						RefreshTime(0);
					}
					else if(staSystem == E_SET_ALARM)//!!!同上
					{
					 	RefreshAlarm();
					}
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
       
    tmp = (SYS_MCLK * ms) / 1000;  
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

    if (tmr200ms >= 200) 
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
