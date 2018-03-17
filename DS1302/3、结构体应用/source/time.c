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

u8 setIndex = 0;//时间设置索引
struct sTime buffTime;//时间缓冲区

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

u8 IncBcd(u8 bcd, u8 i)
{
	if(i == 1)//0~23
	{
		bcd += 0x01;
		if((bcd&0x0F) == 0x0A)
		{
			bcd &= 0xF0;
		 	bcd += 0x10;
		}
		if(bcd == 0x24)
			bcd = 0x00;
	}
	else//0~59
	{
		bcd += 0x01;
		if((bcd&0x0F) == 0x0A)
		{
			bcd &= 0xF0;
		 	bcd += 0x10;
		}
		if(bcd == 0x60)
			bcd = 0x00;
	} 

	return bcd;	
}

u8 DecBcd(u8 bcd, u8 i)
{
	if(i == 1)//0~23
	{
		bcd -= 0x01;
		if((bcd&0x0F) == 0x0F)
		{
		 	bcd = (bcd&0xF0)|0x09;
		}
		if(bcd == 0xF9)
		{
		 	bcd = 0x23;
		}
	}
	else//0~59
	{
		bcd -= 0x01;
		if((bcd&0x0F) == 0x0F)
		{
		 	bcd = (bcd&0xF0)|0x09;
		}
		if(bcd == 0xF9)
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

void RightShiftTimeSet()
{
 	if(setIndex != 0)
	{
		if(setIndex < 3)
			setIndex++;
		else
			setIndex = 1;
		RefreshSet();
	}	
}

void LeftShiftTimeSet()
{
 	if(setIndex != 0)
	{
		if(setIndex > 1)
			setIndex--;
		else
			setIndex = 3;
		RefreshSet();
	}	
}

void EnterTimeSet()
{
	setIndex = 1;
	RefreshSet();
	LedOpenCursor();
}

void ExitTimeSet(bit save)
{
 	setIndex = 0;
	if(save)
	{
	 	SetRealTime(&buffTime);
	}
	LedCloseCursor();
}

void KeyAction(u8 keycode)
{
 	if((keycode >= '0') && (keycode <= '9'))
	{
	}
	else if(keycode == 0x19)//增
	{
	 	IncTimeSet();
	}
	else if(keycode == 0x18)//减
	{
	 	DecTimeSet();
	}
	else if(keycode == 0x17)//左移
	{
	 	LeftShiftTimeSet();
	}
	else if(keycode == 0x16)//右移
	{
	 	RightShiftTimeSet();
	}
	else if(keycode == 0x0D)
	{
	 	if(setIndex == 0)
		{
			EnterTimeSet();
		}
		else
		{
		 	ExitTimeSet(1);//退出时间设置并保存
		}
	}
	else if(keycode == 0x1B)
	{
	 	ExitTimeSet(0);//退出时间设置不保存!
	}
}