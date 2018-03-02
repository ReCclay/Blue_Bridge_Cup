/*
*******************************************************************************
* 文件名：sonic.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#include "config.h"
#include <intrins.h>
#include "main.h"


void Delay13us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void InitSonic()
{
	TMOD &= 0x0F;
	TMOD |= 0x10;
	TF1 = 0;
	TR1 = 0;		
}

void SendWave()
{
 	u8 i = 8;
	
	while(i--)
	{
		Sonic_Txd = 1;
		Delay13us();
		Sonic_Txd = 0;
		Delay13us();
	}
}

void SonicDriver()//数码管显示
{
	u16 time, distance;

	SendWave();//发送8个40Khz脉冲信号
	TH1 = 0; //清零计数值准备开始
	TL1 = 0;
	TR1 = 1;
	while((Sonic_Rxd) && (TF1==0));
	TR1 = 0;
	
	if(TF1 == 1)
	{
	 	TF1 = 0;
		LedBuff[0] = 0xBF; //对应显示横线
		LedBuff[1] = 0xBF;
		LedBuff[2] = 0xBF;
		LedBuff[3] = 0xBF;
	}
	else 
	{
	 	time = (TH1 * 256) + TL1;
		distance = (u16)((time * 0.17 * 12) / 11.0592); //[机器周期*定时器计时*10^(-6)](s) * 340(m/s)/2 * 10^(2); 单位厘米，且有一位小数点！
		LedBuff[0] = LedChar[distance%10];				  
		LedBuff[1] = LedChar[distance/10%10];
		LedBuff[1] &= 0x7F;	//点亮小数点
		LedBuff[2] = LedChar[distance/100%10];
		LedBuff[3] = LedChar[distance/1000%10];
	}
}
