#include "config.h"
#include "led.h"
#include "keyboard.h"
#include "ds18b20.h"
#include "time.h"

u8 T0RH, T0RL;
bit flag200ms = 0;
bit flag400ms = 0;
bit flag800ms = 0;

void CloseFucker();
void ConfigTimer0(u16 ms);
void ChangeLed();

void main()
{
 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	Start18B20();

	while(1)
	{
	 	KeyDriver();	

		if(flag200ms)//200ms定时到
		{
			flag200ms = 0;
			TempMonitor();

			if(!SetSta)
			{
				if(TempScale == 2)//当前温度处于区间2
				{
					ChangeLed();//灯以0.2s为间隔闪烁
					P0 = 0x00;
					OpenRelay();//打开继电器	 	
				}
			}
		}
		if(!SetSta)//设置状态下L1无动作
		{
			if(flag400ms)//400ms定时到
			{
				flag400ms = 0;
				if(TempScale == 1)//当前温度处于区间1
				{
					ChangeLed();//灯以0.4s为间隔闪烁
					P0 = 0x00;//关了蜂鸣器
					CloseRelay();	
				}
			}
			if(flag800ms)//800ms定时到
			{
				flag800ms = 0;
				if(TempScale == 0)//当前温度处于区间0
				{
				 	ChangeLed();//灯以0.8s为间隔闪烁
					P0 = 0x00;
					CloseRelay();	
				}
			}
		}	
	}	
}

void ChangeLed()
{
 	static bit LedSta = 0;
	
	if(LedSta == 0)
	{
		LedSta = 1;
		P0 = 0xFF;//避免无关灯影响
		OpenLed(1);	
	}
	else
	{
		LedSta = 0;
		P0 = 0xFF;
		CloseLed(1);	
	}	
}

void CloseFucker()//初始化蜂鸣器和继电器（关闭状态）
{
 	P2 = (P2&0x1F)|0xA0;
	P0 = P0&0xAF;
	P2 = P2&0x1F;
}

void ConfigTimer0(u16 ms)
{
	u32 tmp;

	tmp = 11059200 / 12;
	tmp = (tmp*ms) / 1000;
	tmp = 65536 - tmp;
	T0RH = (u8)(tmp >> 8);
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
	static u16 tmr400ms = 0;
	static u16 tmr800ms = 0;

	TH0 = T0RH;
	TL0 = T0RL;

	tmr200ms++;
	tmr400ms++;
	tmr800ms++;

	if(tmr200ms >= 200)
	{
	 	tmr200ms = 0;
		flag200ms = 1;
	}
	if(tmr400ms >= 400)
	{
	 	tmr400ms = 0;
		flag400ms = 1;
	}
	if(tmr800ms >= 800)
	{
	 	tmr800ms = 0;
		flag800ms = 1;
	}
	
	LedScan();
	KeyScan();	 	
}