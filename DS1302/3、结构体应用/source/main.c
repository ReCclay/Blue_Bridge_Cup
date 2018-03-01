/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：来看下封装成结构体如何实现电子钟！
*         
*******************************************************************************
*/

#include "config.h"
#include "ds1302.h"
#include "lcd1602.h"
#include "keyboard.h"

u8 T0RH = 0, T0RL = 0;
u8 setIndex = 0;//时间设置索引
bit flag200ms = 1;
struct sTime buffTime;


void ConfigTimer0(u16 ms);
void CloseFucker();
void RefreshTimeShow();

void main()
{
	u8 psec = 0xAA;

	
	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();
	InitLcd1602();

	LcdShowStr(3, 0, "20  -  -  ");
	LcdShowStr(4, 1, "  :  :  ");
	

	while(1)
	{
		KeyDriver();
		if(flag200ms && (setIndex == 0)) //200ms定时到，且未处于设置模式
		{
		 	flag200ms = 0;
			GetRealTime(&buffTime);
			if(psec != buffTime.sec)
			{
			 	RefreshTimeShow();
				psec = buffTime.sec;
			}
		}	
	}
}

void ShowBcdByte(u8 x, u8 y, u8 bcd)
{
 	u8 str[4];

	str[0] = (bcd >> 4) + '0';
	str[1] = (bcd & 0x0F) + '0';
	str[2] = '\0';
	LcdShowStr(x, y, str);
}

void RefreshTimeShow()
{
 	ShowBcdByte(5, 0, buffTime.year);
	ShowBcdByte(8, 0, buffTime.mon);
	ShowBcdByte(11, 0, buffTime.day);
	ShowBcdByte(4, 1, buffTime.hour);
	ShowBcdByte(7, 1, buffTime.min);
	ShowBcdByte(10, 1, buffTime.sec);
}

void RefreshSetShow()
{
	switch(setIndex)
	{
	 	case 1: LcdSetCursor(5, 0);  break;
		case 2: LcdSetCursor(6, 0);  break;
		case 3: LcdSetCursor(8, 0);  break;
		case 4: LcdSetCursor(9, 0);  break;
		case 5: LcdSetCursor(11, 0); break;
		case 6: LcdSetCursor(12, 0); break;
		case 7: LcdSetCursor(4, 1);  break;
		case 8: LcdSetCursor(5, 1);  break;
		case 9: LcdSetCursor(7, 1);  break;
		case 10: LcdSetCursor(8, 1);  break;
		case 11: LcdSetCursor(10, 1); break;
		case 12: LcdSetCursor(11, 1); break;
		default: break;
	} 	
}

u8 IncBcdHigh(u8 bcd)
{
	if((bcd & 0xF0) < 0x90)
	 	bcd += 0x10;	
	else
		bcd &= 0x0F;

	return bcd;
}

u8 IncBcdLow(u8 bcd)
{
	if((bcd & 0x0F) < 0x09)
	 	bcd += 0x01;
	else
		bcd &= 0xF0;

	return bcd;
}

u8 DecBcdHigh(u8 bcd)
{
	if((bcd & 0xF0) > 0x00)
	 	bcd -= 0x10;	
	else
		bcd |= 0x90;

	return bcd;
}

u8 DecBcdLow(u8 bcd)
{
	if((bcd & 0x0F) > 0x00)
		bcd -= 0x01;
	else
		bcd |= 0x09;

	return bcd;
}

void IncSetTime()
{
 	switch(setIndex)
	{
	 	case 1: buffTime.year = IncBcdHigh(buffTime.year); break;
		case 2: buffTime.year = IncBcdLow(buffTime.year);  break;
		case 3: buffTime.mon  = IncBcdHigh(buffTime.mon);  break;
		case 4: buffTime.mon  = IncBcdLow(buffTime.mon);   break;
		case 5: buffTime.day  = IncBcdHigh(buffTime.day);  break;
		case 6: buffTime.day  = IncBcdLow(buffTime.day);   break;
		case 7: buffTime.hour = IncBcdHigh(buffTime.hour); break;
		case 8: buffTime.hour = IncBcdLow(buffTime.hour);  break;
		case 9: buffTime.min  = IncBcdHigh(buffTime.min);  break;
		case 10: buffTime.min = IncBcdLow(buffTime.min);   break;
		case 11: buffTime.sec = IncBcdHigh(buffTime.sec);  break;
		case 12: buffTime.sec = IncBcdLow(buffTime.sec);   break;
	    default : break;
	}
	RefreshTimeShow();
	RefreshSetShow();
}
void DecSetTime()
{
	switch(setIndex)
	{
	 	case 1: buffTime.year = DecBcdHigh(buffTime.year); break;
		case 2: buffTime.year = DecBcdLow(buffTime.year);  break;
		case 3: buffTime.mon  = DecBcdHigh(buffTime.mon);  break;
		case 4: buffTime.mon  = DecBcdLow(buffTime.mon);   break;
		case 5: buffTime.day  = DecBcdHigh(buffTime.day);  break;
		case 6: buffTime.day  = DecBcdLow(buffTime.day);   break;
		case 7: buffTime.hour = DecBcdHigh(buffTime.hour); break;
		case 8: buffTime.hour = DecBcdLow(buffTime.hour);  break;
		case 9: buffTime.min  = DecBcdHigh(buffTime.min);  break;
		case 10: buffTime.min = DecBcdLow(buffTime.min);   break;
		case 11: buffTime.sec = DecBcdHigh(buffTime.sec);  break;
		case 12: buffTime.sec = DecBcdLow(buffTime.sec);   break;
	    default : break;
	}
	RefreshTimeShow();
	RefreshSetShow();
}

void RightShiftTimeSet()
{
 	if(setIndex != 0)
	{
		if(setIndex < 12)
			setIndex++;
		else
			setIndex = 1;
		RefreshSetShow();
	}	
}

void LeftShiftTimeSet()
{
 	if(setIndex != 0)
	{
		if(setIndex > 1)
			setIndex--;
		else
			setIndex = 12;
		RefreshSetShow();
	}	
}

void EnterTimeSet()
{
 	setIndex = 2;
	LeftShiftTimeSet();
	LcdOpenCursor();
}

void ExitTimeSet(bit save)
{
 	setIndex = 0;
	if(save)
	{
	 	SetRealTime(&buffTime);
	}
	LcdCloseCursor();
}

void KeyAction(u8 keycode)
{
 	if((keycode >= '0') && (keycode <= '9'))
	{
	}
	else if(keycode == 0x26)
	{
	 	IncSetTime();
	}
	else if(keycode == 0x28)
	{
	 	DecSetTime();
	}
	else if(keycode == 0x25)
	{
	 	LeftShiftTimeSet();
	}
	else if(keycode == 0x27)
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

void CloseFucker()
{
 	P2 = (P2 & 0x1F) | 0xA0;
	P0 = P0 & 0xAF;
	P2 = P2&0x1F;
}


void ConfigTimer0(u16 ms)
{
    u32 tmp; 
    
    tmp = 11059200 / 12;    
    tmp = (tmp * ms) / 1000;  
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
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH; 
    TL0 = T0RL;
	KeyScan();
    tmr200ms++;
    if (tmr200ms >= 200) 
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
}
