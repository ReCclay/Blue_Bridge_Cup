#include "config.h"
#include "led.h"
#include "pcf8591.h"
#include "ds18b20.h"
#include "e2prom.h"

int CurTemp, bufTemp;
bit staTemp = 0;
u8 MaxTemp = 0;
u8 MinTemp = 0;

void TempMonitor()
{
 	if(bufTemp > MaxTemp)
	{
	 	ET1 = 1;

		P0 = 0x00;
		P2 = (P2&0x1F)|0xA0;
		P0 = P2&0xEF;
		P2 = P2&0x1F;
	}
	else if(bufTemp < MinTemp)
	{
		ET1 = 0;
		PWMOUT = 1;

		P0 = 0x00;
		P2 = (P2&0x1F)|0xA0;
		P0 = P0|0x10;
		P2 = P2&0x1F;
	}
	else
	{
	 	ET1 = 0;
		PWMOUT = 1;

		P0 = 0x00;
		P2 = (P2&0x1F)|0xA0;
		P0 = P2&0xEF;
		P2 = P2&0x1F;
	}	
}

void ChangeTemp()
{	
	E2Write(&MaxTemp, 0x00, 1);
	E2Write(&MinTemp, 0x01, 1);	
}
void RefreshLim()
{
	LedBuff[7] = LedChar[MaxTemp/10];
	LedBuff[6] = LedChar[MaxTemp%10];
	LedBuff[5] = LedChar[MinTemp/10];
	LedBuff[4] = LedChar[MinTemp%10]; 
}

void RefreshTemp()
{
	Get18B20Temp(&CurTemp);
	Start18B20();
	CurTemp >>= 4;

	if((CurTemp >0) && (CurTemp < 99))
	{
	 	bufTemp = CurTemp;	
	}

	LedBuff[1] = LedChar[bufTemp/10];
	LedBuff[0] = LedChar[bufTemp%10];	
}


void KeyAction(u8 keycode)
{
	staTemp = 1;
	if(keycode == '1')//S7
	{
		MinTemp--;
	}
	else if(keycode == '4')//S6
	{
		MaxTemp--;
	}
	else if(keycode == '7')//S5
	{
		MinTemp++;
	}
	else if(keycode == '0')//S4
	{
	 	MaxTemp++;	
	}	
}