#include "config.h"
#include "keyboard.h"
#include "led.h"
#include "time.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "eeprom.h"
#include "pcf8591.h"


u8 T0RH, T0RL;
bit flag200ms = 0;

void CloseFucker();
void ConfigTimer0(u16 ms);

void main()
{
	u8 val=0;
//	u8 y=29;
//	u8 x=0;
//	u16 i;
	
 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();

//	E2Write(&y, 0x24, 1);
//	for(i=0; i<3000; i++);
//	E2Read(&x, 0x24, 1);
//	ShowNumber(x);
	SetDACOut(48);

	while(1)
	{
		KeyDriver();
		
	 	if(flag200ms)
		{
		 	flag200ms = 0;
//			RefreshTemp();
//			RefreshTime(1);
//			val = GetADCVal(3);
//			ShowNumber(val);
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
	tmp = (tmp*ms) / 1000;
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
	
	KeyScan();
	LedScan();	
}