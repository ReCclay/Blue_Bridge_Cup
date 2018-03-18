/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.1.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#include "config.h"
#include "keyboard.h"
#include "led.h"
#include "ds1302.h"
#include "time.h"
#include "main.h"
#include "keyboard.h"

u8 T0RH, T0RL;
bit flag200ms = 0;
bit flag1s = 0;
enum eSystemSta SystemSta = E_NORMAL;

void ConfigTimer0(u16 ms);
void CloseFucker();


void main()
{
	bit sta=0;//闪烁所用的标志位

 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();

	while(1)
	{
	 	KeyDriver();
		if(flag200ms)
		{
			AlarmMonitor();
			flag200ms = 0;
			if((SystemSta == E_NORMAL) && (!flag18B20))
			{
				RefreshTime(1);	 	
			}
		 	else if((SystemSta == E_NORMAL) && (flag18B20))
			{
			 	Refresh18B20();
			}					
		}
		if(flagCursor)
		{
			if(flag1s)
			{
				flag1s = 0;
				if(sta == 0)//灭
				{
				 	sta = 1;
					LedBuff[setCursorIndex] = 0xFF;
					LedBuff[setCursorIndex-1] = 0xFF;	
				}
				else//亮
				{
				 	sta = 0;
					if(SystemSta == E_SET_TIME)
					{
					 	RefreshTime(0);
					}
					if(SystemSta == E_SET_ALARM)
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
	P2 = P2 & 0X1F;
}

void ConfigTimer0(u16 ms)
{
 	u32 tmp;

	tmp = (SYS_MCLK*ms)/1000;
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

