/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：红外接收，while(1)里面红外扫描接收，T1计时高低电平时间！
*         接收完毕后，在while(1)进行数码管显示。
*******************************************************************************
*/

#include "config.h"
#include "infrared.h"

u8 code LedChar[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
u8 LedBuff[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

u8 T0RH = 0, T0RL = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();

void main()
{	
	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitInfrared();

	while(1)
	{
		if(!IR_INPUT)
		{
			IRScan();
		}
		if(irflag)
		{
		 	irflag = 0;
			LedBuff[5] = LedChar[ircode[0] >> 4];//用户码
			LedBuff[4] = LedChar[ircode[0]&0x0F];
			LedBuff[1] = LedChar[ircode[2] >> 4];//键码
			LedBuff[0] = LedChar[ircode[2]&0x0F];
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
 	static u8 index=0;

	P2 = (P2 & 0x1F) | 0xE0;
	P0 = 0xFF;
	P2 = (P2 & 0x1F);

	P2 = (P2 & 0x1F) | 0xC0;
	P0 = 0x80>>index;
	P2 = (P2 & 0x1F);
	P2 = (P2 & 0x1F) | 0xE0;
	P0 = LedBuff[index];
	P2 = (P2 & 0x1F);
	
	index++;
	index &= 0x07;
}

void InterruptTimer0() interrupt 1
{
    TH0 = T0RH; 
    TL0 = T0RL;

	LedScan();
}
