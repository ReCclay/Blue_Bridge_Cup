/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：按键上 - 加电压； 按键下 - 减电压！
*         
*******************************************************************************
*/

#include "config.h"
#include "i2c.h"
#include "keyboard.h"

u8 T0RH = 0, T0RL = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();

void main()
{
	EA = 1;
	CloseFucker();
	ConfigTimer0(1);

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
	static u8 volt = 0;
	
	if(keycode == 0x26)
	{
	 	if(volt < 50)
		{
		 	volt++;
			SetDACOut((volt*255)/50);//输入数字量
		}
	}	
	else if(keycode == 0x28)
	{
	 	if(volt > 0)
		{
		 	volt--;
			SetDACOut((volt*255)/50);//输入数字量
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

void InterruptTimer0() interrupt 1
{ 
    TH0 = T0RH; 
    TL0 = T0RL;
	KeyScan();
}
