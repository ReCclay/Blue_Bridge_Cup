/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：使用蓝桥的底层!
*         
*******************************************************************************
*/

#include "config.h"
#include "ds1302.h"

u8 T0RH = 0;
u8 T0RL = 0;
bit flag200ms = 0;

u8 code LedChar[] = {
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
	0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E
};
u8 LedBuff[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


void ConfigTimer0(u16 ms);
void CloseFucker();

void main()
{
	u8 psec = 0xAA;
	u8 time[8];

	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();
	
	LedBuff[2] = 0xBF;
	LedBuff[5] = 0xBF;

	while(1)
	{
		if(flag200ms)
		{
		 	flag200ms = 0;
			DS1302BurstRead(time);
			if(psec != time[0])
			{
                LedBuff[7] = LedChar[time[2] >> 4];  //“时”
                LedBuff[6] = LedChar[time[2]&0x0F];
                LedBuff[4] = LedChar[time[1] >> 4];  //“分”
                LedBuff[3] = LedChar[time[1]&0x0F];
                LedBuff[1] = LedChar[time[0] >> 4];  //“秒”
                LedBuff[0] = LedChar[time[0]&0x0F];
                
                psec = time[0];  //用当前值更新上次秒数
			}
		}		
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
void LedScan()
{
 	static u8 index = 0;

	P2 = (P2&0x1F) | 0xE0;
	P0 = 0xFF;
	P2 = P2&0x1F;
	P2 = (P2&0x1F) | 0xC0;
	P0 = 0x80>>index;
	P2 = P2&0x1F;
	P2 = (P2&0x1F) | 0xE0;
	P0 = LedBuff[index];
	P2 = P2&0x1F;

	index++;
	index &= 0x07;
}

void InterruptTimer0() interrupt 1
{
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH; 
    TL0 = T0RL;
	LedScan();
    tmr200ms++;
    if (tmr200ms >= 200) 
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
}
