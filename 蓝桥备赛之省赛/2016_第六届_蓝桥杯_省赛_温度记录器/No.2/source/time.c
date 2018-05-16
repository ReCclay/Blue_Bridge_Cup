/*
*******************************************************************************
* 文件名：time.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#include "config.h"
#include "led.h"
#include "ds1302.h"
#include "ds18b20.h"
#include "main.h"

struct sTime buffTime;
u8 SetSec = 1;//默认采集间隔1s
int TempTable[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bit flagtempOver = 0;//十个温度采集完成标志
bit flagLed = 0;//LED开关
u8 tempIndex = 0;//采集温度索引 00 ~ 09


void RefreshTime(bit sta)
{ 	
	
 	if(sta == 1)
	{
	 	GetRealTime(&buffTime);
	}
	if(TurnSta)
	{
	 	LedBuff[2] = 0xBF;
		LedBuff[5] = 0xBF;	
	}
	else
	{
	 	LedBuff[2] = 0xFF;
		LedBuff[5] = 0xFF;	
	}

	LedBuff[0] = LedChar[(buffTime.sec)&0x0F];
	LedBuff[1] = LedChar[(buffTime.sec)>>4];
	LedBuff[3] = LedChar[(buffTime.min)&0x0F];
	LedBuff[4] = LedChar[(buffTime.min)>>4];	
	LedBuff[6] = LedChar[(buffTime.hour)&0x0F];
	LedBuff[7] = LedChar[(buffTime.hour)>>4];
}

void RefreshTemp()
{
 	int temp;
	u8 i;	

	Start18B20();
	Get18B20Temp(&temp);
	
	temp = temp >> 4;

	for(i=7; i>=3; i--)
	{
	 	LedBuff[i] = 0xFF;
	}
	LedBuff[1] = LedChar[temp%10];
	LedBuff[2] = LedChar[temp/10];
	LedBuff[0] = 0xC6;
}

void CollectTemp()
{
	static u8 i=0;
	int temp;

	if(i<10)
	{
	 	Start18B20();
		if(Get18B20Temp(&temp))//别忘了利用它的返回值
		{
			temp >>= 4;//利用中间变量装入数组！
			TempTable[i] = temp;
			i++;
		}	
	}
	else
	{
		i = 0;
		flagtempOver = 1;//一次温度全部采集完成标志
		flagLed = 1;//打开LED开关
	 	staSys = E_DISPLAY;//系统模式改变	
	}
}

void RefreshSetSec()
{
 	u8 i;

	for(i=7; i>=3; i--)
	{
	 	LedBuff[i] = 0xFF;
	}
	LedBuff[2] = 0xBF;
	LedBuff[1] = LedChar[SetSec/10];
	LedBuff[0] = LedChar[SetSec%10];
}

void EnterSec()
{
 	static u8 SecIndex = 0;//采集温度的时间间隔的索引值

	switch(SecIndex)
	{
	 	case 0: SetSec = 5;  break;
		case 1: SetSec = 30; break;
		case 2: SetSec = 60; break;
		case 3: SetSec = 1; break;
		default: break;
	}
	SecIndex++;
	SecIndex &= 0x03;

	RefreshSetSec();
}

void EnterCLK()
{	
	staSys = E_CLK;
	RefreshTime(1);
}

void EnterTemp(bit sta)//温度采集显示界面
{
	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[tempIndex/10];
	LedBuff[5] = LedChar[tempIndex%10];
	LedBuff[4] = 0xFF;
	LedBuff[3] = 0xFF;
	LedBuff[2] = 0xBF;
	LedBuff[1] = LedChar[TempTable[tempIndex]/10];
	LedBuff[0] = LedChar[TempTable[tempIndex]%10];

	if(sta)
	{
		tempIndex++;
		if(tempIndex == 10)
		{
		 	tempIndex = 0;	
		}
	}
}

void KeyAction(u8 keycode)
{
 	if(keycode == '0')//对应S4按键
	{
		if(staSys == E_SELECT)//当前的系统模式参数设置
		{
			EnterSec();
		}	
	}
	else if(keycode == '7')//对应S5键
	{
		if(staSys == E_SELECT)//当前的系统模式是参数设置
		{
	 		EnterCLK();
		}	
	}
	else if(keycode == '4')//对应S6键
	{
		if(staSys == E_DISPLAY)//当前的系统模式是温度采集界面
		{
			flagLed = 0;//关闭闪烁的LED
			P2 = (P2&0x1F) | 0x80;//确保此时LED是灭的！
			P0 = 0xFF;
			P2 = P2&0x1F;
			 
	 		EnterTemp(1);//进行温度显示切换
		}
	}
	else if(keycode == '1')//对应S7键
	{
	 	if(staSys == E_DISPLAY)//当前的系统模式温度采集界面
		{
		 	staSys = E_SELECT;//重新设置成参数设置界面
			SetSec = 1;//恢复初始化的时间设置间隔
			tempIndex = 0;//温度索引
			flagtempOver = 0;//温度全部采集完成标志
			RefreshSetSec();//刷新回到参数设置界面
		}
	}

}