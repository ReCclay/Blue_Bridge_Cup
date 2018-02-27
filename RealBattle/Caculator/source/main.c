/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#include "config.h"
#include "lcd1602.h"
#include "keyboard.h"


long num1=0, num2=0, result=0;
u8 step=0, oprt=0;
u8 T0RH, T0RL;

void ConfigTimer0(u16 ms);

void main()
{
	EA = 1;
	ConfigTimer0(1);
	InitLcd1602();
	LcdShowStr(15, 1, "0");

	while(1)
	{
		KeyDriver();
	}

}

u8 LongToStr(u8 *str, long dat)
{
	u8 i=0;
	u8 len=0;
	u8 buf[12];
	
	if(dat < 0)
	{
	 	dat = -dat;
		len++;
		*str++ = '-';
	}
	do{
		buf[i++] = dat % 10;
		dat /= 10;
	}while(dat > 0);
	len += i;
	
	while(i-- > 0)
	{
		*str++ = buf[i] + '0';
	}		
	*str = '\0';

	return len;
}

void Reset()
{
	step = 0;
	num1 = 0;
	num2 = 0;
	result = 0;	
	LcdFullClear();
}

void NumKeyAction(u8 num)
{
	u8 buf[16];
	u8 len;

 	if(step > 1)
	{
	 	Reset();
	}
	if(step == 0)
	{
	 	num1 = num1 * 10 + num;
		len = LongToStr(buf, num1);
		LcdShowStr(16-len, 1, buf);
	}
	else
	{
	 	num2 = num2 * 10 + num;
		len = LongToStr(buf, num2);
		LcdShowStr(16-len, 1, buf);
	}
}

void ShowOprt(u8 y, u8 type)
{
 	switch(type)
	{
	 	case 0: LcdShowStr(0, y, "+"); break;
		case 1: LcdShowStr(0, y, "-"); break;
		case 2: LcdShowStr(0, y, "*"); break;
		case 3: LcdShowStr(0, y, "/"); break;
		default: break;
	}
	oprt = type;
}

void OprtKeyAction(u8 type)
{
	u8 len;
	u8 buf[12];

	if(step == 0)
	{
	 	step = 1;
		ShowOprt(1, type);
		len = LongToStr(buf, num1);
		LcdShowStr(16-len, 0, buf);
		LcdShowStr(15, 1, "0");
		LcdAreaClear(1, 1, 14);
	}
}



void GetResult()
{
	u8 len;
	u8 buf[12];
	
	if(step == 1) //最好加上这个！
	{
	 	switch(oprt)
		{
		 	case 0: result = num1 + num2; break;
			case 1: result = num1 - num2; break;
			case 2: result = num1 * num2; break;
			case 3: result = num1 / num2; break;
			default: break;
		}
		
		step = 2;
		ShowOprt(0, oprt);
		LcdShowStr(0, 1, "=");
		len = LongToStr(buf, result);
		LcdAreaClear(1, 1, 15-len);
		LcdShowStr(16-len, 1, buf);
	}
}

void KeyAction(u8 keycode)
{
	if((keycode >= '0') && (keycode <= '9'))
	{
	 	NumKeyAction(keycode - '0');	
	} 	
	else if(keycode == 0x26)
	{
	 	OprtKeyAction(0);
	}
	else if(keycode == 0x28)
	{
		OprtKeyAction(1);
	}
	else if(keycode == 0x25)
	{
	 	OprtKeyAction(2);
	}
	else if(keycode == 0x27)
	{
	 	OprtKeyAction(3);
	}
	else if(keycode == 0x1B)
	{
	 	Reset();
		LcdShowStr(15, 1, "0");
	}
	else if(keycode == 0x0D)
	{
		GetResult();	 	
	}
}

void ConfigTimer0(u16 ms)
{
	u32 tmp;

	tmp = 11059200 / 12;
	tmp = (tmp * ms) / 1000;
	tmp = 65536 - tmp;
	T0RH = (u8)(tmp >> 8);
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
 	TH0 = T0RH;
	TL0 = T0RL;

	KeyScan();
}