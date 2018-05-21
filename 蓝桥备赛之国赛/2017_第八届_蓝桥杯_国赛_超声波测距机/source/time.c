#include "config.h"
#include "led.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "main.h"
#include "eeprom.h"

struct sTime buffTime;

u8 ViewIndex = 0;
u8 SaveDataCnt = 0;

void RefreshTime(bit sta)
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

void RefreshTemp()
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

void CloseLed()
{
	LedBuff[0] = 0xFF;
	LedBuff[1] = 0xFF;
	LedBuff[2] = 0xFF;
	LedBuff[3] = 0xFF;
	LedBuff[4] = 0xFF;
	LedBuff[5] = 0xFF;
	LedBuff[6] = 0xFF;
	LedBuff[7] = 0xFF;
}

void RefreshView()
{
	u8 dat;
	
	LedBuff[7] = LedChar[ViewIndex/10];
	LedBuff[6] = LedChar[ViewIndex%10];
	
	E2Read(&dat, ViewIndex, 1);
	LedBuff[0] = LedChar[dat%10];
	LedBuff[1] = LedChar[dat/10%10];
	LedBuff[2] = LedChar[dat/100%10];
}

void RefreshSet()//刷新设置修正距离界面
{
	LedBuff[7] = 0x8E;
	
	LedBuff[0] = LedChar[FixDistance%10];
	LedBuff[1] = LedChar[FixDistance/10];
}

void FixData()//修正数据
{
	if(SysSta == E_RUN)//如果当前系统状态是距离测量，然后转入修正界面
	{
		SysSta = E_SET;
		P2 = (P2&0x1F) | 0x80;//开启LED8
		P0 = 0x7F;
		P2 = (P2&0x1F);
		CloseLed();
		RefreshSet();
	}
	else if(SysSta == E_SET)//如果当前系统的状态是修正界面，那就重新回到距离测量界面
	{
		SysSta = E_RUN;
		P2 = (P2&0x1F) | 0x80;//关闭LED8
		P0 = 0xFF;
		P2 = (P2&0x1F);
	}
}

void SaveData()
{
	u8 dat;
	
	if(distance < 255)
	{
		dat = distance;//傻逼了我，把赋值方向都搞错了。。。
	}
	
	E2Write(&dat, SaveDataCnt, 1);
	OpenLed1 = 1;//保存完一次数据打开LED1的开关
	SaveDataCnt++;//10个数据计数
	
	if(SaveDataCnt >= 10)//超过10个数据，覆盖
	{
		SaveDataCnt = 0;
	}
}

void VeiwData()
{
	if(SysSta == E_RUN)
	{
		SysSta = E_VIEW;//从距离测量界面切换到存储显示界面
		P2 = (P2&0x1F) | 0x80;//开启LED7
		P0 = 0xBF;
		P2 = (P2&0x1F);
		SaveDataCnt = 0;//清空距离测量界面下存储数据计数变量
		
		CloseLed();
		RefreshView();
	}
	else if(SysSta == E_VIEW)
	{
		SysSta = E_RUN;
		P2 = (P2&0x1F) | 0x80;//关闭LED7
		P0 = 0xFF;
		P2 = (P2&0x1F);
		ViewIndex   = 0;//退出存储显示界面同时清零索引变量
	}
}

void KeyAction(u8 keycode)
{
 	if(keycode == '0')
	{
		if(SysSta == E_RUN)
		{
			SaveData();//存储当前距离
			Led1Cnt = 0;//这个清零原因 - LED1每存一个数据的闪烁持续1s，为了让连续两次保存的间隔小于1s时，LED1还能根据最后一次按重新设置持续时间！
		}
	}
	else if(keycode == '7')
	{
		VeiwData();//查看存储的数据
	}
	else if(keycode == '1')
	{
		if(SysSta == E_VIEW)
		{
			ViewIndex++;//存储数据翻页
			if(ViewIndex >= 10)
			{
				ViewIndex = 0;
			}
			RefreshView();
			
		}
		if(SysSta == E_SET)
		{	//修正距离自加
			FixDistance += 10;
			
			if(FixDistance >= 100)
			{
				FixDistance = 0;
			}
			
			RefreshSet();
		}
	}
	else if(keycode == '4')
	{//修正距离
		FixData();
	}
}