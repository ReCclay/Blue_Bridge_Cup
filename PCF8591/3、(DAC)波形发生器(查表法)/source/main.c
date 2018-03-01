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
#include "i2c.h"
#include "keyboard.h"

u8 code SinWave[] = {  //正弦波波表
    127, 152, 176, 198, 217, 233, 245, 252,
    255, 252, 245, 233, 217, 198, 176, 152,
    127, 102,  78,  56,  37,  21,   9,   2,
      0,   2,   9,  21,  37,  56,  78, 102,
};
u8 code TriWave[] = {  //三角波波表
      0,  16,  32,  48,  64,  80,  96, 112,
    128, 144, 160, 176, 192, 208, 224, 240,
    255, 240, 224, 208, 192, 176, 160, 144,
    128, 112,  96,  80,  64,  48,  32,  16,
};
u8 code SawWave[] = {  //锯齿波表
      0,   8,  16,  24,  32,  40,  48,  56,
     64,  72,  80,  88,  96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184,
    192, 200, 208, 216, 224, 232, 240, 248,
};

u8 T0RH = 0, T0RL = 0;
u8 T1RH = 0, T1RL = 0;
u8 *pWave;

void ConfigTimer0(u16 ms);
void CloseFucker();
void SetWaveFreq(u8 freq);

void main()
{
	EA = 1;
	CloseFucker();
	ConfigTimer0(1);

	pWave = SinWave;//上电默认为频率为10的正弦波
	SetWaveFreq(10);

	while(1)
	{
		KeyDriver();	
	}
}

void SetDACOut(u8 val)
{
 	I2CStart();
	if(!I2CWrite(0x48<<1))
	{
	 	I2CStop();
		return;
	}
	I2CWrite(0x40);
	I2CWrite(val);
	I2CStop();
}

void KeyAction(u8 keycode)
{
	static u8 i=0;

	if(keycode == 0x26)
	{
	 	if(i == 0)
		{
		 	i = 1;
			pWave = TriWave; 	
		}
		else if(i == 1)
		{
			i = 2;
			pWave = SawWave;
		}
		else if(i == 2)
		{
		 	i = 0;
			pWave = SinWave;
		}

	}
}

void CloseFucker()
{
 	P2 = (P2 & 0x1F) | 0xA0;
	P0 = P0 & 0xAF;
	P2 = P2&0x1F;
}

void SetWaveFreq(u8 freq)
{
	u32 tmp;

	tmp = (11059200/12) / (freq*32); //计数器计数频率是波形频率的32倍！
	tmp = 65536 - tmp;
	T1RH = (u8)(tmp>>8);
	T1RL = (u8)tmp;
	TMOD &= 0x0F;
	TMOD |= 0x10;
	TH1 = T1RH;
	TL1 = T1RL;
	ET1 = 1;
	PT1 = 1; //设置高优先级！
	TR1 = 1;
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
    TH0 = T0RH; 
    TL0 = T0RL;
	KeyScan();
}

void InterrupTimer1() interrupt 3
{
 	static u8 i=0;

	TH1 = T1RH;
	TL1 = T1RL;
	SetDACOut(pWave[i]);
	i++;
	if(i >= 32)
	{
	 	i=0;
	}
}
