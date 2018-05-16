#include "config.h"

u8 LedChar[] = {
	0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90
};
u8 LedBuff[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

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

void ShowNumber(u16 num)//修改为流转间隔专用！
{
 	u8 buf[4];
	char i;

	for(i=0; i<4; i++)
	{
		buf[i] = num%10;
		num /= 10;
	}
	for(i=3; i>0; i--)
	{
	 	if(buf[i] == 0)
		{
		 	LedBuff[i] = 0xFF;
		}
		else
		{
		 	break;
		}
	}
	for( ; i>=0; i--)
	{
	 	LedBuff[i] = LedChar[buf[i]];
	}
}