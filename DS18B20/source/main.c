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
#include "ds18b20.h"

u8 T0RH = 0, T0RL = 0;
bit flag1s=0;

void ConfigTimer0(u16 ms);
void CloseFucker();
u8 IntToString(u8 *str, int dat);

void main()
{
	bit res;
	int temp;
	int intT, decT;
	u8 len;
	u8 str[12];

	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
   	Start18B20();
	InitLcd1602();


	while(1)
	{
		if(flag1s)
		{
		 	flag1s = 0;
			res = Get18B20Temp(&temp);
			if(res)
			{
			 	intT = temp >> 4;
				decT = temp & 0x0F;
				len = IntToString(str, intT);
				str[len++] = '.';
				decT = (decT*10) / 16;
				str[len++] = decT + '0';
				while(len < 0)
				{
				 	str[len++] = ' ';
				}
				str[len] = '\0';
				LcdShowStr(0, 0, str);
			}
			else
			{
			 	LcdShowStr(0, 0, "error");
			}
			Start18B20();
		}
	}
}

u8 IntToString(u8 *str, int dat)
{
 	u8 len=0;
	u8 buf[12];
	char i=0;

	if(dat < 0)
	{
		dat = -dat;
		len++;
		*str++ = '-';
	}
	do
	{
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
    static unsigned int tmr1s = 0;
    
    TH0 = T0RH; 
    TL0 = T0RL;
    tmr1s++;
    if (tmr1s >= 1000) 
    {
        tmr1s = 0;
        flag1s = 1;
    }
}
