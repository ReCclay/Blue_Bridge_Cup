#include "config.h"
#include "led.h"
#include "pcf8591.h"

bit staWater = 0;
u16 WaterCnt = 0;
u16 PriceCnt = 0;

void ADCMonitor()
{
 	u8 val;

	val = GetADCValue(1);
	if(val < 12)
	{
	 	P2 = (P2&0x1F)|0x80;
		P0 = 0xFE;
		P2 = P2&0x1F;
	}
	else
	{
	 	P2 = (P2&0x1F)|0x80;
		P0 = 0xFF;
		P2 = P2&0x1F;
	}
}				   

void RefreshPrice()
{
 	LedBuff[7] = 0xFF;
	LedBuff[6] = LedChar[0];
	LedBuff[6] &= 0x7F;
	LedBuff[5] = LedChar[5];
	LedBuff[4] = LedChar[0];
}

void OpenWater()
{
	P0 = 0x00;
	P2 = (P2&0x1F)|0xA0;
	P0 = P0|0x10;
	P2 = P2&0x1F;
}

void CloseWater()
{
	P0 = 0x00;
 	P2 = (P2&0x1F)|0xA0;
	P0 = P0&0xEF;
	P2 = P2&0x1F;	
}

void RefreshPriceCnt()
{
	PriceCnt = WaterCnt * 0.5;

	LedBuff[3] = LedChar[PriceCnt/1000];
	LedBuff[2] = LedChar[(PriceCnt/100)%10];
	LedBuff[1] = LedChar[(PriceCnt/10)%10];
	LedBuff[0] = LedChar[PriceCnt%10];
	LedBuff[2] &= 0x7F;	
}

void RefreshWaterCnt()
{	
	WaterCnt += 10;
	if(WaterCnt >= 9999)
	{
	 	WaterCnt = 9999;
		staWater = 0;
		CloseWater();
		RefreshPriceCnt();
	}
	LedBuff[3] = LedChar[WaterCnt/1000];
	LedBuff[2] = LedChar[(WaterCnt/100)%10];
	LedBuff[1] = LedChar[(WaterCnt/10)%10];
	LedBuff[0] = LedChar[WaterCnt%10];
	LedBuff[2] &= 0x7F;
}

void KeyAction(u8 keycode)
{
	if(keycode == '1')
	{
		if(staWater == 0)
		{
			WaterCnt = 0;
			PriceCnt = 0;
			RefreshWaterCnt();
			staWater = 1;
	 		OpenWater();
		}		
	}
	else if(keycode == '4')
	{
		if(staWater)
		{
			staWater = 0;
			CloseWater();
			RefreshPriceCnt();
		}
	}	

}