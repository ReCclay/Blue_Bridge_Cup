#include "config.h"
#include "led.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "main.h"
#include "keyboard.h"

struct sTime buffTime;

u16 LedCnt;//指示灯的流转间隔，范围400~1200ms
u8 LedMode = 1;//LED的流转模式     范围1~4
bit LedSta = 0;//LED流转开关
//状态区分
u8 StaCnt = 0;//0-运行状态 1-选中运行模式 2-选中流转间隔

void RefreshTime(bit sta)//测试1302底层用的
{
 	if(sta)
	{
	 	GetRealTime(&buffTime);
	}
	LedBuff[7] = LedChar[buffTime.hour>>4];
	LedBuff[6] = LedChar[buffTime.hour&0x0F];
	LedBuff[5] = 0xBF;
	LedBuff[4] = LedChar[buffTime.min>>4];
	LedBuff[3] = LedChar[buffTime.min&0x0F];
	LedBuff[2] = 0xBF;
	LedBuff[1] = LedChar[buffTime.sec>>4];
	LedBuff[0] = LedChar[buffTime.sec&0x0F];
}

void RefreshTemp()//测试18b20底层用的
{
	int temp, tempBuff;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;

	if((temp>0) && (temp<99))
	{
	 	tempBuff = temp;
	}

	LedBuff[1] = LedChar[tempBuff/10];
	LedBuff[0] = LedChar[tempBuff%10];
}

void CloseLed()//关闭所有的数码管
{
	LedBuff[7] = 0xFF;
	LedBuff[6] = 0xFF;
	LedBuff[5] = 0xFF;
	LedBuff[4] = 0xFF;
	LedBuff[3] = 0xFF;
	LedBuff[2] = 0xFF;
	LedBuff[1] = 0xFF;
	LedBuff[0] = 0xFF;
}

void RefreshSet()//刷新处于设置界面下的数码管
{
	//处于设置界面的数码管状态
	LedBuff[7] = 0xBF;
	//运行模式显示
	LedBuff[6] = LedChar[LedMode];
	LedBuff[5] = 0xBF;
	LedBuff[4] = 0xFF;
	
	//流转间隔显示
	ShowNumber(LedCnt);
}

void RefreshLevel()//等级界面刷新显示
{
	LedBuff[7] = 0xFF;
	LedBuff[6] = 0xFF;
	LedBuff[5] = 0xFF;
	LedBuff[4] = 0xFF;
	LedBuff[3] = 0xFF;
	LedBuff[2] = 0xFF;
	
	LedBuff[1] = 0xBF;
	LedBuff[0] = LedChar[LedLevel];
}

void SetLedCnt()//设置LED的运行模式和流转间隔界面
{	
	if(StaCnt < 2)
	{
		StaCnt++;
		SysSta = E_SET;
		
		LedCnt = ModeLedCnt[LedMode-1];
		RefreshSet();//刷新设置界面下的数码管
	}
	else 
	{
		//返回正常运行状态，关闭全部数码管
		StaCnt = 0;
		SysSta = E_RUN;
		
		CloseLed();//关闭全部数码管
	}
}

void UintAdd()//对应单元(运行模式、流转间隔)的加
{
	if(StaCnt == 1)
	{
		if(LedMode < 4)
		{
			LedMode++;
			
			LedCnt = ModeLedCnt[LedMode-1];
			RefreshSet();
		}
		else
		{
			LedMode = 1;
			
			LedCnt = ModeLedCnt[0];
			RefreshSet();
		}
		
	}
	else if(StaCnt == 2)
	{
		if(LedCnt < 1200)
		{
			LedCnt += 100;
			ModeLedCnt[LedMode-1] = LedCnt;
			RefreshSet();
		}
	}
}

void UintSub()//对应单元(运行模式、流转间隔)的减
{
	if(StaCnt == 1)
	{
		if(LedMode > 1)
		{
			LedMode--;
			
			LedCnt = ModeLedCnt[LedMode-1];
			RefreshSet();
		}
		else
		{
			LedMode = 4;
			
			LedCnt = ModeLedCnt[3];
			RefreshSet();
		}
		
	}
	else if(StaCnt == 2)
	{
		if(LedCnt > 400)
		{
			LedCnt -= 100;
			ModeLedCnt[LedMode-1] = LedCnt;
			RefreshSet();
		}
	}
}

void KeyAction(u8 keycode)//按键动作函数
{
	if(keycode == '1')//LED流转的启动和停止
	{
		LedSta = ~LedSta;
	}
	else if(keycode == '4')//LED运行模式及流转间隔设置
	{
		SetLedCnt();
	}
	else if(keycode == '7')//加
	{
		if(SysSta == E_SET)
		{
			UintAdd();
		}	
	}
	else if(keycode == '0')//减
	{
		if(SysSta == E_SET)
		{
			UintSub();
		}
	}
}

void Mode1()//从左往右流
{
	static u8 cnt = 0;
		
	if(cnt <= 7)
	{
		P2 = (P2&0x1F) | 0x80;
		P0 = ~(0x01<<cnt);
		PWMSta = ~(0x01<<cnt);//P0口的状态暂存，用到定时器的PWM控制
		P2 = P2&0x1F;
		cnt++;
	}
	else
	{
		cnt = 0;
		LedMode = 2;
	}
}

void Mode2()//从右往左流
{
	static u8 cnt = 1;
		
	if(cnt <= 7)
	{
		P2 = (P2&0x1F) | 0x80;
		P0 = ~(0x80>>cnt);
		PWMSta = ~(0x80>>cnt);
		P2 = P2&0x1F;
		cnt++;
	}
	else
	{
		cnt = 1;
		LedMode = 3;
	}	
}

void Mode3()//从两边往中间流
{
	static u8 cnt = 0;
	
	if(cnt == 0)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0x7E;
		PWMSta = 0x7E;
		P2 = P2&0x1F;
	}
	else if(cnt == 1)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xBD;
		PWMSta = 0xBD;
		P2 = P2&0x1F;
	}
	else if(cnt == 2)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xDB;
		PWMSta = 0xDB;
		P2 = P2&0x1F;
	}
	else if(cnt == 3)
	{
		cnt = 0;
		LedMode = 4;
		
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xE7;
		PWMSta = 0xE7;
		P2 = P2&0x1F;
	}
	else{}	
}

void Mode4()//从中间往两边流
{
	static u8 cnt = 0;
	
	if(cnt == 0)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xE7;
		PWMSta = 0xE7;
		P2 = P2&0x1F;
	}
	else if(cnt == 1)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xDB;
		PWMSta = 0xDB;
		P2 = P2&0x1F;
	}
	else if(cnt == 2)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xBD;
		PWMSta = 0xBD;
		P2 = P2&0x1F;
	}
	else if(cnt == 3)
	{
		cnt = 0;
		LedMode = 1;
		
		P2 = (P2&0x1F) | 0x80;
		P0 = 0x7E;
		PWMSta = 0x7E;
		P2 = P2&0x1F;
	}
	else{}	
}

void LedDriver()//指示灯模式
{	
	//注意了如果改成else if 切换会产生停顿的现象
	
	if(LedMode==1)
	{
		LedCnt = ModeLedCnt[0];
		Mode1();
	}
	if(LedMode==2)
	{
		LedCnt = ModeLedCnt[1];
		Mode2();
	}
	if(LedMode==3)
	{
		LedCnt = ModeLedCnt[2];
		Mode3();
	}
	if(LedMode==4)
	{
		LedCnt = ModeLedCnt[3];
		Mode4();
	}
}



