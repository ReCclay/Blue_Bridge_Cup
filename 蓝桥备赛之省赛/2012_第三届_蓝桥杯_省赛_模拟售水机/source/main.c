#include "config.h"
#include "led.h"
#include "time.h"
#include "pcf8591.h"
#include "keyboard.h"

bit flag1s;
bit flag200ms;

u8 T0RH, T0RL;

void CloseFucker();
void ConfigTimer0(u16 ms);
 
void main()
{
 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	RefreshPrice();
	RefreshPriceCnt();
	

	while(1)
	{
		KeyDriver();
		if(flag1s)
		{
			flag1s = 0;
			if(staWater)
			{
				RefreshWaterCnt();
			}
		}

		if(flag200ms)
		{
		 	flag200ms = 0;
			RefreshPrice();
			ADCMonitor();
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

void InterruptTimer0() interrupt 1
{
	static u16 tmr1s = 0;
	static u8 tmr200ms = 0;

 	TH0 = T0RH;
	TL0 = T0RL;
 	tmr1s++;
	tmr200ms++;

   	if(tmr1s >= 1000)
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