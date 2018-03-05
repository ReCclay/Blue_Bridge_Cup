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
#include "main.h"
#include "ds18b20.h"

bit staMute = 0;//静音标志位 0-非静音 1-静音
bit flagAlarm = 0;//闹钟定时到的标志

u8 AlarmHour = 0x00;//闹钟-时分秒
u8 AlarmMin  = 0x00;
u8 AlarmSec  = 0x00;
struct sTime buffTime;//时间缓冲区

struct sTime setTime;//！！！设置模式下，备份时间的缓冲区

u8 setIndex = 0; //初始化设置索引为0，	0-时钟显示 1-设置'时' 2-设置'分' 3-设置'秒'
bit flagCursorCnt = 0;

u8 AlarmCnt = 0;//!!!!!!!!!!!!!!!!!!!			  

void AlarmMonitor()
{
	static bit AlarmSta = 0;

 	if((buffTime.hour == AlarmHour) && (buffTime.min == AlarmMin) && (buffTime.sec == AlarmSec))
	{ //!!!此刻时钟时间和闹钟时间一致
		flagAlarm = 1;
	}
	if(flagAlarm)
	{
		AlarmCnt++;
		if(!staMute)//当前是非静音模式
		{
			if(!AlarmSta)//当前是灭的状态
			{
				AlarmSta = 1;
			 	P2 = (P2&0x1F) | 0x80;
				P0 = 0xFE;
				P2 = P2&0x1F;
			}	
			else 
			{
				AlarmSta = 0;
			 	P2 = (P2&0x1F) | 0x80;
				P0 = 0xFF;
				P2 = P2&0x1F;
			}
		}
		if(AlarmCnt == 25)///!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			AlarmCnt = 0;
		 	staMute = 1;
			flagAlarm = 0;
			P2 = (P2&0x1F)|0x80;
			P0 = 0xFF;
			P2 = P2&0x1F;
		}
	}
}

void ShowLedNumber(u8 index, u8 num)
{
	if(num == 0xBF)//第2、5数码管的'-'特殊处理
	{
	 	LedBuff[index] = 0xBF;
	}
	else
	{
	 	LedBuff[index] = LedChar[num];
	}
}

void RefreshTime(bit sta)
{
	if(sta==1)//1表示获取实时时间的刷新
	{
	 	GetRealTime(&buffTime);//！！！注意这个刷新时间的封装是带获取实时时间的
	}
	ShowLedNumber(7, buffTime.hour>>4);
	ShowLedNumber(6, buffTime.hour&0x0F);
	ShowLedNumber(5, 0xBF);
	ShowLedNumber(4, buffTime.min>>4);
	ShowLedNumber(3, buffTime.min&0x0F);
	ShowLedNumber(2, 0xBF);
	ShowLedNumber(1, buffTime.sec>>4);
	ShowLedNumber(0, buffTime.sec&0x0F);
}

void RefreshAlarm()
{
 	ShowLedNumber(7, AlarmHour>>4);
	ShowLedNumber(6, AlarmHour&0x0F);
	ShowLedNumber(5, 0xBF);
	ShowLedNumber(4, AlarmMin>>4);
	ShowLedNumber(3, AlarmMin&0x0F);
	ShowLedNumber(2, 0xBF);
	ShowLedNumber(1, AlarmSec>>4);
	ShowLedNumber(0, AlarmSec&0x0F);	
}

void RefreshSet()//刷新光标显示的位置
{
	switch(setIndex)// 1-时 2-分 3-秒
	{
	 	case 1: LedSetCursor(1);  break;
		case 2: LedSetCursor(2);  break;
		case 3: LedSetCursor(3);  break;
		default: break;
	} 	
}

void Refresh18B20()
{
 	int temp;
	u8 i;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;//舍弃小数位
	
	for(i=7; i>=3; i--)
	{
	 	LedBuff[i] = 0xFF;
	}

	LedBuff[2] = LedChar[temp/10];//！！！竟然忘了LedChar...
	LedBuff[1] = LedChar[temp%10];
	LedBuff[0] = 0xC6; //对应字符'C'
}

u8 IncBcd(u8 bcd, u8 i)
{
	if(i == 1)//0~23
	{
		if(bcd < 0x09)
		 	bcd += 0x01;
		else if(bcd == 0x09)
		 	bcd = 0x10;
		else if(bcd < 0x19)
		 	bcd += 0x01;
		else if(bcd == 0x19)
			bcd = 0x20;
		else if(bcd < 0x23)
		 	bcd += 0x01;	 	
		else 
		 	bcd = 0x00;
	}
	else//0~59
	{
	 	if(bcd < 0x09)
			bcd += 0x01;
		else if(bcd == 0x09)
			bcd = 0x10;
		else if(bcd < 0x19)
			bcd += 0x01;
		else if(bcd == 0x19)
			bcd = 0x20;
		else if(bcd < 29)
			bcd += 0x01;
		else if(bcd == 0x29)
			bcd = 0x30;
		else if(bcd < 39)
			bcd += 0x01;
		else if(bcd == 0x39)
			bcd =0x40;
		else if(bcd < 0x49)
			bcd += 0x01;
		else if(bcd == 0x49)
			bcd = 0x50;
		else if(bcd < 0x59)
			bcd+= 0x01;
		else
			bcd = 0x00;
	} 

	return bcd;	
}
u8 DecBcd(u8 bcd, u8 i)
{
	if(i == 1)//0~23
	{
		if(bcd > 0x20)
			bcd -= 0x01;
		else if(bcd == 0x20)
			bcd = 0x19;
		else if(bcd > 0x10)
			bcd -= 0x01;
		else if(bcd == 0x10)
			bcd = 0x09;
		else if(bcd > 0x00)
			bcd -= 0x01;
		else 
			bcd = 0x23;
	}
	else//0~59
	{
		if(bcd > 0x50)
			bcd -= 0x01;
		else if(bcd == 0x50)
			bcd = 0x49;
		else if(bcd > 0x40)
			bcd -= 0x01;
		else if(bcd == 0x40)
			bcd = 0x39;
		else if(bcd > 0x30)
			bcd -= 0x01;
		else if(bcd == 0x30)
			bcd = 0x29;
		else if(bcd > 0x20)
			bcd -= 0x01;
		else if(bcd == 0x20)
			bcd = 0x19;
		else if(bcd > 0x10)
			bcd -= 0x01;
		else if(bcd == 0x10)
			bcd = 0x09;
		else if(bcd > 0x00)
			bcd -= 0x01;
		else 
			bcd = 0x59;
	} 

	return bcd;		
}

void IncTimeSet()
{
 	switch(setIndex)
	{
		case 1: buffTime.hour = IncBcd(buffTime.hour, 1);  break;
		case 2: buffTime.min  = IncBcd(buffTime.min,  0);  break;
		case 3: buffTime.sec  = IncBcd(buffTime.sec,  0);  break;
	    default : break;
	}
	RefreshTime(0);
}

void IncAlarmSet()
{
 	switch(setIndex)
	{
		case 1: AlarmHour = IncBcd(AlarmHour, 1);  break;
		case 2: AlarmMin  = IncBcd(AlarmMin,  0);  break;
		case 3: AlarmSec  = IncBcd(AlarmSec,  0);  break;
	    default : break;
	}
	RefreshAlarm();
}

void DecTimeSet()
{
 	switch(setIndex)
	{
		case 1: buffTime.hour = DecBcd(buffTime.hour, 1); break;
		case 2: buffTime.min  = DecBcd(buffTime.min,  0);  break;
		case 3: buffTime.sec  = DecBcd(buffTime.sec,  0);  break;
	    default : break;
	}
	RefreshTime(0);
}

void DecAlarmSet()
{
 	switch(setIndex)
	{
		case 1: AlarmHour = DecBcd(AlarmHour, 1);  break;
		case 2: AlarmMin  = DecBcd(AlarmMin,  0);  break;
		case 3: AlarmSec  = DecBcd(AlarmSec,  0);  break;
	    default : break;
	}
	RefreshAlarm();
}

void SwitchTimeSet()
{																
 	if(staSystem == E_NORMAL)
	{
	 	staSystem = E_SET_TIME;
		setIndex = 1;//设置索引对应为 - '时'
		
		setTime.hour = buffTime.hour;//！！！
		setTime.min = buffTime.min;//！！！
		setTime.sec = buffTime.sec;//！！！

		RefreshSet();
		flagCursorCnt = 0;
		LedOpenCursor();
	}	
	else if(staSystem == E_SET_TIME)
	{
	 	setIndex++;
		if(setIndex < 4)
		{
		 	RefreshSet();
		}
		else//一旦加到4，表示退出时间设置，进入时钟显示模式。
		{
		 	setIndex = 0; 
			staSystem = E_NORMAL;//！！！换到4别忘了回到时钟显示模式！
			if((setTime.hour == buffTime.hour) && (setTime.min == buffTime.min) && (setTime.sec == buffTime.sec))
			{//！！！虽然进入时间设置模式，但是根本没改变时间！
			 	RefreshTime(1);
			}
			else
			{//！！！改变了时间
				SetRealTime(&buffTime);//保存设置的时间
			}
			LedCloseCursor();//关闭光标
		}	
	}
}

void SwitchAlarmSet()
{																
 	if(staSystem == E_NORMAL)
	{
	 	staSystem = E_SET_ALARM;
		setIndex = 1;//设置索引对应为 - '时'
		RefreshSet();
		flagCursorCnt = 0;
		LedOpenCursor();
	}	
	else if(staSystem == E_SET_ALARM)
	{
	 	setIndex++;
		if(setIndex < 4)
		{
		 	RefreshSet();
		}
		else//一旦加到4，表示退出闹钟设置，进入时钟显示模式。
		{
		 	setIndex = 0; 
			staSystem = E_NORMAL;//！！！换到4别忘了回到时钟显示模式！
			staMute = 0;//!!!6666666666666一次闹钟静音之后，再设置一次即可再定一个闹钟！
			RefreshTime(1);//闹钟切换到时钟显示模式，直接刷新就行！
			LedCloseCursor();//关闭光标
		}	
	}
}


void KeyAction(u8 keycode)
{
	if((staSystem == E_NORMAL) && flagAlarm)//按下任意键满足1、当前模式是“时钟显示” 2、闹钟定时到 静音
	{
	 	staMute = 1;
		flagAlarm = 0;
		AlarmCnt = 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!
		P2 = (P2&0x1F)|0x80;
		P0 = 0xFF;
		P2 = P2&0x1F;
	}

	else if(keycode == '1')//S7
	{
	 	SwitchTimeSet();	
	}
	else if(keycode == '4')//S6
	{
		SwitchAlarmSet();
	}
	else if(keycode == '7')//S5
	{
		if(staSystem == E_SET_TIME)
		{
		 	IncTimeSet();
		}
		else if(staSystem == E_SET_ALARM)
		{
		 	IncAlarmSet();
		}
	}
	else if(keycode == '0')//S4
	{
		if(staSystem == E_SET_TIME)
		{
		 	DecTimeSet();
		}
		else if(staSystem == E_SET_ALARM)
		{
		 	DecAlarmSet();
		}
	}
}
