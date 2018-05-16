#include "config.h"
#include "led.h"
#include "ds1302.h"
#include "main.h"
#include "e2prom.h"
#include "pcf8581.h"

bit flagRelay = 0;
bit BuzzSta = 0;//手动模式下的蜂鸣器开关控制位
char LimitValue = 50;
char SetLimitValue = 50;//阈值设置缓冲区
bit SetLimitSta = 0;//处于阈值设置状态就是1
struct sTime buffTime;
u8 val;

//体会下面位操作的骚
void OpenRelay()
{	
	P2 = (P2&0x1F) | 0xA0;
	P0 = P0|0x10;
	P2 = P2&0x1F; 	
}
void CloseRelay()
{
	P2 = (P2&0x1F) | 0xA0;
	P0 = P0&0xEF;
	P2 = P2&0x1F; 
}

void OpenBuzz()
{
	P2 = (P2&0x1F) | 0xA0;
	P0 = P0|0x40;
	P2 = P2&0x1F;	
}

void CloseBuzz()
{
	P0 = 0x00;
	P2 = (P2&0x1F) | 0xA0;
	P0 = P0&0xBF;
	P2 = P2&0x1F;
}	

void LimitMonitorAUTO()
{
	val = GetADCValue(3);
	if(val*2 < LimitValue)//乘2的原因是题目上所言的比例系数!!! H 湿度 = KV Rb2
	{
		P0 = 0x00;//这句的意义是关闭AUTO状态下的蜂鸣器
	 	OpenRelay();
	} 	
	else
	{
		P0 = 0x00;//这句的意义是关闭AUTO状态下的蜂鸣器
	 	CloseRelay();
	}
}

void LimitMonitorHAND()
{
	val = GetADCValue(3);
	if((!BuzzSta) && (val*2 < LimitValue))
	{
		OpenBuzz();	
	}
	else
	{
		CloseBuzz();
	}

	if(flagRelay)
	{
		OpenRelay(); 	
	}
	else
	{
		CloseRelay();	
	}
}

void RefreshTime(bit sta)
{
 	if(sta == 1)
	{
	 	GetRealTime(&buffTime);
	}

	LedBuff[7] = LedChar[buffTime.hour>>4];//注意时间是BCD码必须的这样写，而普通的变量直接取余或者除法就行
	LedBuff[6] = LedChar[buffTime.hour&0x0F];
	LedBuff[5] = 0xBF;
	LedBuff[4] = LedChar[buffTime.min>>4];
	LedBuff[3] = LedChar[buffTime.min&0x0F];
	LedBuff[2] = 0xFF;
	LedBuff[1] = LedChar[LimitValue/10];
	LedBuff[0] = LedChar[LimitValue%10];

}

void SwitchMode()
{
	if(SysSta == E_AUTO)
	{
	 	SysSta = E_HAND;
		OpenLed(2);	
	}
	else if(SysSta == E_HAND)
	{
	 	SysSta = E_AUTO;
		OpenLed(1);	
	}
}

void RefreshAdjust()
{
	LedBuff[7] = 0xBF;
	LedBuff[6] = 0xBF;
	LedBuff[5] = 0xFF;
	LedBuff[4] = 0xFF;
	LedBuff[3] = 0xFF;
	LedBuff[2] = 0xFF;
	LedBuff[1] = LedChar[SetLimitValue/10];
	LedBuff[0] = LedChar[SetLimitValue%10];  	
}

void SetLimit()
{
	if(SetLimitSta == 0)
	{
		SetLimitSta = 1;
		RefreshAdjust();			
	}
	else
	{
	 	SetLimitSta = 0;
		LimitValue = SetLimitValue;
		E2Write(&SetLimitValue, 0x00, 1);//设置好的阈值写入E2中
	}
}

void ChangeBuzz()
{		
	if(BuzzSta == 0)
	{
	 	BuzzSta = 1;
	}
	else
	{
	 	BuzzSta = 0;
	}
}

void KeyAction(u8 keycode)
{
 	if(keycode == '1')
	{
		SwitchMode(); 	
	}

	if(SysSta == E_AUTO)
	{
		if(keycode == '4')
		{
		 	SetLimit();
		}
		if(SetLimitSta)//处于自动的系统状态，并且当前是设置阈值的界面
		{
			if(keycode == '7')
			{
			 	SetLimitValue++;
				if(SetLimitValue > 99)//注意边界处理
				{
				 	SetLimitValue = 99;
				}
			}
			else if(keycode == '0')
			{
			 	SetLimitValue--;
				if(SetLimitValue <= 0)
				{
				 	SetLimitValue = 0;
				}
			}	
		}
	}
	else if(SysSta == E_HAND)
	{
		if(keycode == '4')
		{
		 	ChangeBuzz();
		} 
		else if(keycode == '7')
		{
			flagRelay = 1;
		}	
		else if(keycode == '0')
		{
			flagRelay = 0;
		}
	}		
}