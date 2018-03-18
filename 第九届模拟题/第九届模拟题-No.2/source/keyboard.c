/*
*******************************************************************************
* 文件名：keyboard.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#include "config.h"
#include "time.h"

bit flag18B20 = 0;

u8 pdata KeySta[4][4] = {
	{1, 1, 1, 1},  {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
};

u8 code KeyCodeMap[4][4] = {
	{'1', '2',  '3',  0x19},
	{'4', '5',  '6',  0x18},
	{'7', '8',  '9',  0x17},
	{'0', 0x1B, 0x0D, 0x16}
};

void KeyDriver()
{
 	u8 i, j;
	static u8 pdata backup[4][4] = {
		{1, 1, 1, 1},  {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
	};

	for(i=0; i<4; i++)
	{
	 	for(j=0; j<4; j++)
		{
		 	if(KeySta[i][j] != backup[i][j])
			{
			 	if(backup[i][j] != 0)
				{
					KeyAction(KeyCodeMap[i][j]);	
				}
				backup[i][j] = KeySta[i][j];
			}
		}
	}
}

void KeyScan()
{
	u8 i;
	static u8 keyout = 0;
 	static u8 pdata keybuff[4][4] = {
		{0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}
	};
	
	switch(keyout)
	{
	 	case 0: KEY_OUT_1 = 0; KEY_OUT_4 = 1; break;
		case 1: KEY_OUT_2 = 0; KEY_OUT_1 = 1; break;
		case 2: KEY_OUT_3 = 0; KEY_OUT_2 = 1; break;
		case 3: KEY_OUT_4 = 0; KEY_OUT_3 = 1; break;
		default: break;
	} 
	
	keybuff[keyout][0] = (keybuff[keyout][0] << 1) | KEY_IN_1;
	keybuff[keyout][1] = (keybuff[keyout][1] << 1) | KEY_IN_2;
	keybuff[keyout][2] = (keybuff[keyout][2] << 1) | KEY_IN_3;
	keybuff[keyout][3] = (keybuff[keyout][3] << 1) | KEY_IN_4;	

	for(i=0; i<4; i++)
	{
	 	if((keybuff[keyout][i] & 0x0F) == 0x0F)
		{
		 	KeySta[keyout][i] = 1;
			if((keyout == 3) && (i == 0))
			{
				flag18B20 = 0;
			}
		}
		else if((keybuff[keyout][i] & 0x0F) == 0x00)
		{
		 	KeySta[keyout][i] = 0;
			if((keyout == 3) && (i == 0))
			{
				flag18B20 = 1;
			}
		}
		else 
		{}
	}

	keyout++;
	keyout &= 0x03;

}