/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：AIN0对应右边排针，AIN1对应光敏电阻,AIN3对应滑变！
*         
*******************************************************************************
*/

#include "config.h"
#include "lcd1602.h"
#include "i2c.h"

u8 T0RH = 0, T0RL = 0;
bit flag300ms = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();
u8 GetADCValue(u8 ch);
void ValueToString(u8 *str, u8 val);

void main()
{
	u8 val;
	u8 str[12];

	EA = 1;
	CloseFucker();
	ConfigTimer0(10);//定时10ms
	InitLcd1602();
	LcdShowStr(0, 0, "AIN0  AIN1  AIN3");

	while(1)
	{
		if(flag300ms)
		{
			flag300ms = 0;

			val = GetADCValue(0);
			ValueToString(str, val);
			LcdShowStr(0, 1, str);

			val = GetADCValue(1);
			ValueToString(str, val);
			LcdShowStr(6, 1, str);

			val = GetADCValue(3);
			ValueToString(str, val);
			LcdShowStr(12, 1, str);
		}
	}
}

u8 GetADCValue(u8 ch)
{
	u8 val;

	I2CStart();
	if(I2CWrite(0x48<<1) == 0) //寻器件地址 - 写
	{
	 	I2CStop();
		return 0;  //这里并不像I2C那样使用break，现在是读东西，读不到就得返回0.
	}
	I2CWrite(0x40 | ch);//对应单端模式 - 通道数
	I2CStart();
	I2CWrite((0x48<<1) | 0x01);//寻器件地址 - 读
	I2CReadACK();
	val = I2CReadNAK();//这次的值，是在下8个SCL输出，所以先空读然后再去NAK读！
	I2CStop();

	return val;	
}

void ValueToString(u8 *str, u8 val)//!!注意这里的处理技巧，把电压扩大了10倍！！！
{
 	val = (val*50) / 255;  //电压5V，256个刻度分成255份！
	str[0] = (val/10) + '0';
	str[1] = '.';
	str[2] = (val%10) + '0';
	str[3] = 'V';
	str[4] = '\0';
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
	static u8 tmr300ms = 0;
  
    TH0 = T0RH; 
    TL0 = T0RL;
	tmr300ms++;
	if(tmr300ms >= 30)
	{
	 	tmr300ms = 0;
		flag300ms = 1;
	}

}
