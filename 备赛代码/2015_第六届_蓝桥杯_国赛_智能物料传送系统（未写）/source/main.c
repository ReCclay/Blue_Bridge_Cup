#include "config.h"
#include "led.h"
#include "keyboard.h"
#include "time.h"
#include "ds1302.h"
#include "ds18b20.h"

bit flag200ms = 0;
u8 T0RH, T0RL;

void CloseFucker();
void ConfigTimer0(u16 ms);

void main()
{
 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();
	Start18B20();

	while(1)
	{
		KeyDriver();
	 	if(flag200ms)
		{
		 	flag200ms = 0;
			RefreshTemp();
		}	
	}	
}

void CloseFucker()
{
	P2 = (P2&0x1F) | 0xA0;
	P0 &= 0xAF;
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
	static u16 tmr200ms = 0;

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