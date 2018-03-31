/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：有一个小问题就是因为ADC采集的临界值出会有跳跃，在阈值上下浮动！
*         会造成在手动模式下，AD处于临界阈值附近的时候继电器以及蜂鸣器频繁跳动！
*******************************************************************************
*/

#include "config.h"
#include "led.h"
#include "e2prom.h"
#include "keyboard.h"
#include "ds1302.h"
#include "time.h"
#include "pcf8581.h"
#include "main.h"

enum eSysSta SysSta = E_AUTO;//系统工作状态
bit flag200ms = 1;
u8 T0RH, T0RL;

void CloseFucker();
void ConfigTimer0(u16 ms);

void main()
{
 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();

	OpenLed(1);//打开LED1

	while(1)
	{
		KeyDriver();
		if(flag200ms)
		{
			flag200ms = 0;
			if(!SetLimitSta)
			{
		 		RefreshTime(1);
			}
			else
			{
			 	RefreshAdjust();
			}

			if(SysSta == E_AUTO)//自动模式
			{
			 	LimitMonitorAUTO();	
			}	
			else//手动模式
			{
				LimitMonitorHAND();	
			}
		}

	}
}

void CloseFucker()
{
 	P2 = (P2&0x1F) | 0xA0;
	P0 = P0&0xAF;
	P2 = P2&0x1F;
}

void ConfigTimer0(u16 ms)
{
 	u32 tmp;

	tmp = 11059200/12;
	tmp = (tmp*ms)/1000;
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
	
 	TH0 = T0RH;
	TL0 = T0RL;
	tmr200ms++;

	if(tmr200ms >= 200)
	{
	 	tmr200ms = 0;
		flag200ms = 1;
	}

	LedScan();
	KeyScan();
}