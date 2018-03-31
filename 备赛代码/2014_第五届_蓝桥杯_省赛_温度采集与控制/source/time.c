#include "config.h"
#include "led.h"
#include "ds18b20.h"

bit SetSta = 0;
u8 TempScale = 0;//温度区间
u8 MaxTemp = 28;//默认温度区间 20~30
u8 MinTemp = 20;
char SetMaxTemp = 0;
char SetMinTemp = 0;
u8 SetCnt = 0;

void RefreshTemp()//温度显示界面
{
 	int temp;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;

	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[TempScale];
	LedBuff[5] = 0xBF;
	LedBuff[4] = 0xFF;
	LedBuff[3] = 0xFF; 
	LedBuff[2] = 0xFF;
	LedBuff[1] = LedChar[temp/10];
	LedBuff[0] = LedChar[temp%10];
}

void EnterSet()//设置界面的初始化
{
	LedBuff[7] = 0xBF;
	LedBuff[6] = 0xFF;
	LedBuff[5] = 0xFF;

	LedBuff[4] = 0xFF; 
	LedBuff[3] = 0xFF;

	LedBuff[2] = 0xBF;
	LedBuff[1] = 0xFF;
	LedBuff[0] = 0xFF;		    	
}

void RefreshSetTemp()//设置显示界面刷新
{
 	LedBuff[6] = LedChar[SetMaxTemp/10];//数码管显示温度上限
	LedBuff[5] = LedChar[SetMaxTemp%10];

	LedBuff[1] = LedChar[SetMinTemp/10];//数码管显示温度下限
	LedBuff[0] = LedChar[SetMinTemp%10];
}

void TempMonitor()
{
 	int temp;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;
	
	if(temp < MinTemp)
	{
		TempScale = 0;	 	
	}	
	else if(temp > MaxTemp)
	{
	 	TempScale = 2;
	}
	else
	{
	 	TempScale = 1;
	}

	if(!SetSta)//当前没有处于设置模式，刷新温度界面
	{
		RefreshTemp();
	}
}

void SaveSet()//退出设置界面，保存设置的数据
{
	if(SetMaxTemp < SetMinTemp)
	{
	 	SetCnt = 0;
		SetMaxTemp = 0;
		SetMinTemp = 0;
		P0 = 0xFF;
		OpenLed(2);	
	}
	else
	{
		SetSta = 0;
		MaxTemp = SetMaxTemp;
		MinTemp = SetMinTemp;
		SetCnt = 0;//退出界面记得把上下限的索引位清0
		SetMaxTemp = 0;//体会到再一个最大最小缓冲区的好处了吧。。。
		SetMinTemp = 0; 
		P0 = 0xFF;
		CloseLed(2);
	}	
}

void ChangeTemp(u8 keycode)
{
	if(SetCnt <= 1)//	0/1表示设置温度上限
	{
		SetMaxTemp = (SetMaxTemp * 10) + (keycode - '0');
		if(SetMaxTemp > 99)
		{
		 	SetMaxTemp = 99;	
		} 	
	}
	else if(SetCnt <= 3)//	2/3表示设置温度下限
	{
	 	SetMinTemp = (SetMinTemp * 10) + (keycode - '0');
		if(SetMinTemp > 99)
		{
		 	SetMinTemp = 99;	
		} 		
	}
	SetCnt++;
	RefreshSetTemp();
}

void ClearTemp()
{
	if((SetCnt >= 1) && (SetCnt <= 4))//合法性检验
	{
		SetCnt -= 1;
		if((SetCnt >= 0) && (SetCnt <= 1))
		{
		 	SetMaxTemp /= 10;	
		}
		else if((SetCnt >= 2) && (SetCnt <= 3))
		{
			SetMinTemp /= 10; 	
		}
		else
		{}
		RefreshSetTemp();
	}
}

void KeyAction(u8 keycode)
{
 	if(keycode == 0x1B)//按下设置键
	{
		P0 = 0xFF;
		CloseLed(1);//一旦进入设置页面，关闭LED1
		if(SetSta == 0)
		{
			SetSta = 1;
			EnterSet();//进入设置界面
		}
		else
		{
			SaveSet();//退出设置界面，保存设置的上下限	
		}		
	}
	else if((keycode >= '0') && (keycode <= '9'))
	{
	 	if(SetSta)//按下数字键，当前处于设置模式
		{	
			ChangeTemp(keycode);//输入上下限温度
		}	
	}
	else if(keycode == 0x0D)//按下清除按键
	{
	 	if(SetSta)
		{
			ClearTemp();	 	
		}
	}
}