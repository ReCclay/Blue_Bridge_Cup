#include "config.h"
#include "ds1302.h"
#include "led.h"
#include "ds18b20.h"
#include "main.h"
#include "pcf8591.h"
#include "eeprom.h"

struct sTime buffTime;
u8 VolUpLim = 20;//电压界面的电压上限
u8 VolDoLim = 10;//电压界面的电压下限

u8 SetClkIndex = 0; //0-非设置模式 1-时设置   2-分设置 3-秒设置
u8 SetClkCursor = 0;//0-非光标设置 7-对应时   4-对应分 1-对应秒

u8 SetVolIndex = 0; // 0-非设置模式 1-上限设置 2-下限设置
u8 SetVolCursor = 0;// 0-非光标设置 7-上限设置 3-下限设置

bit FreMode = 0;//0-显示频率 1-显示周期
bit QryMode = 0;//0-触发类型 1-触发时间

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
	int temp, tempBuff;
	
	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;
	if((temp>0) && (temp<99))
	{
		tempBuff = temp;
	}
	LedBuff[1] = LedChar[tempBuff/10];
	LedBuff[0] = LedChar[tempBuff%10];
}

void RefreshVol()
{
	u8 val;
	
	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[1];
	LedBuff[5] = 0xBF;
	LedBuff[4] = 0xFF;
	LedBuff[0] = LedChar[0];
	LedBuff[1] = LedChar[0];
	
	val = GetADCVal(3);
	LedBuff[2] = LedChar[val%10];
	LedBuff[3] = LedChar[val/10];
}

void RefreshVolSet()
{
	LedBuff[0] = LedChar[0];
	LedBuff[1] = LedChar[0];
	LedBuff[4] = LedChar[0];
	LedBuff[5] = LedChar[0];
	
	LedBuff[2] = LedChar[VolDoLim%10];
	LedBuff[3] = LedChar[VolDoLim/10];
	LedBuff[6] = LedChar[VolUpLim%10];
	LedBuff[7] = LedChar[VolUpLim/10];
}

void RefreshFre(bit sta)
{
	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[2];
	LedBuff[5] = 0xBF;
	
	if(sta == 0)//频率小时
	{
		LedBuff[0] = LedChar[FreNum%10];
		LedBuff[1] = LedChar[FreNum/10%10];
		LedBuff[2] = LedChar[FreNum/100%10];
		LedBuff[3] = LedChar[FreNum/1000%10];
		LedBuff[4] = LedChar[FreNum/10000%10];
	}
	else
	{
		TimNum = 1000000 / FreNum;//频率对应的周期，单位us
		
		LedBuff[0] = LedChar[TimNum%10];
		LedBuff[1] = LedChar[TimNum/10%10];
		LedBuff[2] = LedChar[TimNum/100%10];
		LedBuff[3] = LedChar[TimNum/1000%10];
		LedBuff[4] = LedChar[TimNum/10000%10];
	}
}

void RefreshTrig(bit sta)
{
	if(sta == 0)//触发类型显示
	{
		LedBuff[7] = 0xFF;
		LedBuff[6] = 0xFF;
		LedBuff[5] = 0xFF;
		LedBuff[4] = 0xFF;
		LedBuff[3] = 0xFF;
		LedBuff[2] = 0xFF;
		LedBuff[1] = LedChar[0];
		LedBuff[0] = LedChar[TrigType];
	}
	else if(sta == 1)//触发时间显示
	{
		LedBuff[7] = LedChar[TrigHour>>4];
		LedBuff[6] = LedChar[TrigHour&0x0F]; 
		LedBuff[5] = 0xBF;
		LedBuff[4] = LedChar[TrigMin>>4];
		LedBuff[3] = LedChar[TrigMin&0x0F];
		LedBuff[2] = 0xBF;
		LedBuff[1] = LedChar[TrigSec>>4];
		LedBuff[0] = LedChar[TrigSec&0x0F];
	}
}

void ChangeFreMode()//频率和周期的模式切换
{
	if(FreMode == 0)
	{
		FreMode = 1;
	}
	else if(FreMode == 1)
	{
		FreMode = 0;
	}
	RefreshFre(FreMode);
}

void ChangeQryMode()//触发类型和触发时间切换
{
	if(QryMode == 0)
	{
		QryMode = 1;
	}
	else if(QryMode == 1)
	{
		QryMode = 0;
	}
	RefreshTrig(QryMode);
}

u8 IncBcd(u8 bcd, bit i)
{
	bcd += 0x01;
	if((bcd&0x0F) == 0x0A)
	{
		bcd &= 0xF0;
		bcd += 0x10;
	}
	
	if(i == 1)
	{
		if(bcd == 0x24)//0~24
		{
			bcd = 0x00;
		}
	}
	else//0~59
	{
		if(bcd == 0x60)
		{
			bcd = 0x00;
		}
	}
	
	return bcd;
}

u8 DecBcd(u8 bcd, bit i)
{
	bcd -= 0x01;
	if((bcd&0x0F) == 0x0F)
	{
		bcd = (bcd&0xF0) | 0x09;
	}
	
	if(i == 1)
	{
		if(bcd == 0xF9)//注意了0-1=0xFF,但是前面有对低位是F变为9的操作，故这里是判断0xF9！！
		{
			bcd = 0x23;
		}
	}
	else
	{
		if(bcd == 0xF9)
		{
			bcd = 0x59;
		}
	}
	
	return bcd;
}

void UnitAdd()
{
	switch(SetClkIndex)
	{
		case 1: buffTime.hour = IncBcd(buffTime.hour, 1); break;
		case 2: buffTime.min  = IncBcd(buffTime.min,  0); break;
		case 3: buffTime.sec  = IncBcd(buffTime.sec,  0); break;
		default: break;
	}
}

void UnitSub()
{
	switch(SetClkIndex)
	{
		case 1: buffTime.hour = DecBcd(buffTime.hour, 1); break;
		case 2: buffTime.min  = DecBcd(buffTime.min,  0); break;
		case 3: buffTime.sec  = DecBcd(buffTime.sec,  0); break;
		default: break;
	}
}

void LedSetCursor()
{
	if(SetClkIndex == 1)//索引为时
	{
		SetClkCursor = 7;//设置光标对应到时
	}
	else if(SetClkIndex == 2)//索引为分
	{
		SetClkCursor = 4;//设置光标对应到分
	}
	else if(SetClkIndex == 3)//索引为秒
	{
		SetClkCursor = 1;//设置光标对应到秒
	}
	else//非索引情况
	{
		SetClkCursor = 0;//光标归0
	}
}

void VolSetCursor()
{
	if(SetVolIndex == 1)
	{
		SetVolCursor = 7;
	}
	else if(SetVolIndex == 2)
	{
		SetVolCursor = 3;
	}
	else
	{
		SetVolCursor = 0;
	}
}

void VolAdd()
{
	if(SetVolIndex == 1)
	{
		VolUpLim += 5;
		if(VolUpLim >= 95)
		{
			VolUpLim = 95;
		}
		RefreshVolSet();
	}
	else if(SetVolIndex == 2)
	{
		VolDoLim += 5;
		if(VolDoLim >= 95)
		{
			VolDoLim = 95;
		}
		RefreshVolSet();
	}
}

void VolSub()
{
	if(SetVolIndex == 1)
	{
		VolUpLim -= 5;
		if(VolUpLim <= 5)
		{
			VolUpLim = 5;
		}
		RefreshVolSet();
	}
	else if(SetVolIndex == 2)
	{
		VolDoLim -= 5;
		if(VolDoLim <= 5)
		{
			VolDoLim = 5;
		}
		RefreshVolSet();
	}
}

void SaveLimData()
{
	E2Write(&VolUpLim, 0x00, 1);
	E2Write(&VolDoLim, 0x01, 1);
}

void KeyAction(u8 keycode)
{
	if(keycode == '1')//s7 -  时钟
	{
		SysSta = E_CLK;//只要s7按下便更改系统运行状态
		if(flagLedOpen)//巧妙利用flagLedOpen的值进行退出时钟设置模式时更新时钟
		{
			flagLedOpen = 0;//LED闪烁开关打开
			SetRealTime(&buffTime);
		}
		SetClkIndex = 0;//LED闪烁索引清零
		LedSetCursor(); //根据索引设置光标 - 这样做的目的是为了下次进入时钟设置时还能从'时'开始闪烁
						//然后会在while(1)里面进行刷新
	}
	else if(keycode == '0')//s4
	{
		if(SysSta == E_CLK)//当前系统状态是时钟，按下后则进行时间设置
		{
			if(flagLedOpen == 0)//只在开关是关闭的情况下处理
			{
				flagLedOpen = 1;
			}
			
			SetClkIndex++;//处于时钟时钟模式每按下一次s4，进行时分秒的轮询
			if(SetClkIndex >= 4)//边界处理
			{
				SetClkIndex = 1;
			}
			LedSetCursor();//根据索引值设置光标
		}
		
		else if(SysSta == E_VOL)//当前系统状态是电压界面
		{
			if(flagVolOpen == 0) //电压界面LED闪烁开关
			{
				flagVolOpen = 1;
			}
			
			SetVolIndex++;
			if(SetVolIndex >= 3)
			{
				SetVolIndex = 1;
			}
			VolSetCursor();
			RefreshVolSet();//电压设置模式的刷新 （时钟模式没有写这句，是因为在while(1)里面刷新的）
		}
		else if(SysSta == E_FRE)
		{
			ChangeFreMode();
		}
		else if(SysSta == E_QRY)
		{
			ChangeQryMode();
		}
	}
	else if(keycode == '2')//s11 - 加
	{
		if((SysSta == E_CLK) && (SetClkIndex!=0))//处于时钟设置模式 (注意时钟模式 不等于 时钟设置模式)
		{
			UnitAdd();//相应单元进行自加
			RefreshTime(0);
		}
		else if((SysSta == E_VOL) && (SetVolIndex!=0))
		{
			VolAdd();
			RefreshVolSet();
		}
	}
	else if(keycode == '5')//s10 - 减
	{
		if((SysSta == E_CLK) && (SetClkIndex!=0))//处于时钟设置模式 (注意时钟模式 不等于 时钟设置模式)
		{
			UnitSub();//相应单元进行自减
			RefreshTime(0);
		}
		else if((SysSta == E_VOL) && (SetVolIndex!=0))
		{
			VolSub();
			RefreshVolSet();
		}
	}
	else if(keycode == '4')//s6 - 电压测量
	{
		SysSta = E_VOL;//修改系统状态为电压测量
		if(flagVolOpen)//巧妙利用flagVolOpen，轻松识别是否是退出电压参数设置的过程
		{
			flagVolOpen = 0;
			SaveLimData();//保存电压的限值
		}
		RefreshVol();  //电压测量界面显示
		SetVolIndex = 0;
		VolSetCursor();
	}
	else if(keycode == '7')//s5 - 频率测量
	{
		SysSta = E_FRE;
		RefreshFre(FreMode);
	}
	else if(keycode == '8')//s9 - 查询界面
	{
		SysSta = E_QRY;
		RefreshTrig(QryMode);
	}
}