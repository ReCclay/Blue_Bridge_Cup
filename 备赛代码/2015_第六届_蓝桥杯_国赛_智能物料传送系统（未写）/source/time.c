#include "config.h"
#include "led.h"
#include "ds1302.h"
#include "ds18b20.h"

struct sTime buffTime;

void RefreshTime(bit sta)
{
	if(sta)
	{
	 	GetRealTime(&buffTime);
	}		

	LedBuff[7] = LedChar[buffTime.hour>>4];
	LedBuff[6] = LedChar[buffTime.hour&0x0F];
	LedBuff[5] = 0xBF;
	LedBuff[4] = LedChar[buffTime.min>>4];
	LedBuff[3] = LedChar[buffTime.min&0x0F];
	LedBuff[2] = 0xBF;
	LedBuff[1] = LedChar[buffTime.sec>>4];
	LedBuff[0] = LedChar[buffTime.sec&0x0F];
}

void RefreshTemp()
{
 	int temp, bufTemp;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;
	
	if((temp>0) && (temp<99))
	{
	 	bufTemp = temp;
	}

	LedBuff[2] = LedChar[bufTemp/10];
	LedBuff[1] = LedChar[bufTemp%10];
	LedBuff[0] = 0xC6; 
}

void KeyAction(u8 keycode)
{
	static u8 add = 0;

 	if(keycode == '0')
	{
		ShowNumber(++add); 	
	}
}