#include "config.h"
#include "led.h"
#include "time.h"
#include "pcf8591.h"
#include "keyboard.h"
#include "ds18b20.h"
#include "e2prom.h"

bit flag1s = 1;
bit flag200ms = 0;

u8 T0RH, T0RL;
u8 HighRH, HighRL, LowRH, LowRL;

void CloseFucker();
void ConfigTimer0(u16 ms);
void ConfigPWM();

void main()
{
 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	ConfigPWM();//初始上电并不启动PWM
	Start18B20();

	E2Read(&MaxTemp, 0x00, 1);
	E2Read(&MinTemp, 0x01, 1);

	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			RefreshLim();
			TempMonitor();
			if(staTemp)
			{
			 	staTemp = 0;
				ChangeTemp();
			}
		}	
		if(flag1s)
		{
		 	flag1s = 0;
			RefreshTemp();
		}
	}
}

void CloseFucker()
{
 	P2 = (P2&0x1F)|0xA0;
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

void ConfigPWM()
{
 	u32 tmp, high, low;

	tmp = 11059200/12/1000;
	high = tmp*0.3;
	low = tmp - high;
	high = 65536 - high;
	low = 65536 - low;
	HighRH = (u8)(high>>8);
	HighRL = (u8)high;
	LowRH = (u8)(low>>8);
	LowRL = (u8)low;
	TMOD &= 0x0F;
	TMOD |= 0x10;
	TH1 = HighRH;
	TL1 = HighRL;
	ET1 = 0;
	TR1 = 1;
	PWMOUT = 1;
}

void InterruptTimer0() interrupt 1
{
	static u16 tmr1s = 0;
	static u8 tmr200ms = 0;

 	TH0 = T0RH;
	TL0 = T0RL;
	tmr1s++;
	tmr200ms++;

	if(tmr1s >= 500)
	{
	 	tmr1s = 0;
		flag1s = 1;
	}
	if(tmr200ms >= 200)
	{
	 	tmr200ms = 0;
		flag200ms = 1;
	}

	LedScan();
	KeyScan();
}

void InterruptTimer1() interrupt 3
{
 	static bit staPWM = 0;

	if(!staPWM)
	{
	 	staPWM = 1;
		TH1 = LowRH;
		TL1 = LowRL;
		PWMOUT = 0;
	}
	else
	{
		staPWM = 0;
		TH1 = HighRH;
		TL1 = HighRL;
		PWMOUT = 1; 	
	}
}