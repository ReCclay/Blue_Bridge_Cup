#include "config.h"
#include "keyboard.h"
#include "led.h"
#include "time.h"
#include "ds18b20.h"
#include "main.h"

enum estaSys staSys = E_SLEEP;//默认上电睡眠风

u8 T0RH, T0RL;
u8 highRH, highRL, lowRH, lowRL;
bit flag1s = 1;

void CloseFucker();
void ConfigTimer0(u16 ms);
void ConfigTimer1(u8 dc);
void ConfigPWM(u8 dc);

void main()
{
	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	Start18B20();
	ConfigTimer1(20);//对应默认占空比20%
	RefreshMode(0);
	LedTurn(1);//点亮第1个小灯

	while(1)
	{
	 	KeyDriver();
		if(flag1s)
		{
		 	flag1s = 0;
			if(CountTime > 0)
				CountTime--;
			else
			{
			 	ET1 = 0;
				PWMOUT = 0;
			}
			if(staTemp == 0)//如果此时不是温度界面
			{
				 ShowNumber(CountTime);
			}

		}
		if(staTemp)
		{
		 	RefreshTemp();
		}		
	}
}

void CloseFucker()
{
 	P2 = (P2&0x1F)|0xA0;
	P0 = 0xAF;
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

void ConfigTimer1(u8 dc)
{
	u32 tmp;
 	u32 high, low;

	tmp = 11059200/12;
	tmp = (tmp*1)/1000;//频率题目限定为1KHz
	high = (tmp*dc)/100;
	low = tmp - high;
	high = 65536 - high;
	low = 65536 - low;
	highRH = (u8)(high>>8);
	highRL = (u8)high;
	lowRH  = (u8)(low>>8);
	lowRL  = (u8)low;
	TMOD &= 0x0F;
	TMOD |= 0x10;
	TH1 = highRH;
	TL1 = highRL;
	ET1 = 0;//上电默认PWM不输出
	TR1 = 1; 
	PWMOUT = 0;//这里是低电平
}

void ConfigPWM(u8 dc)
{
	u32 tmp;
 	u32 high, low;

	tmp = 11059200/12;
	tmp = (tmp*1)/1000;
	high = (tmp*dc)/100;
	low = tmp - high;
	high = 65536 - high;
	low = 65536 - low;
	highRH = (u8)(high>>8);
	highRL = (u8)high;
	lowRH  = (u8)(low>>8);
	lowRL  = (u8)low;			 		 	
}

void InterruptTimer0() interrupt 1
{
	static u16 tmr1s  = 0;
 	TH0 = T0RH;
	TL0 = T0RL;
	tmr1s++;

	KeyScan();
	LedScan();

	if(tmr1s >= 1000)
	{
		tmr1s = 0;
		flag1s = 1; 
	}
}
void InterruptTimer1() interrupt 3
{
 	static bit staPWM = 0;

	if(staPWM == 0)
	{
		staPWM = 1;
	 	TH1 = lowRH;
		TL1 = lowRL;
		PWMOUT = 0;
	}
	else
	{
	 	staPWM = 0;
		TH1 = highRH;
		TL1 = highRL;
		PWMOUT = 1;//别忘了这个！！！	
	}
}