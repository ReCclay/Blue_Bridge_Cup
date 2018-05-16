/*******************************************************************************
* 文件名：第九届蓝桥省赛_彩灯控制器
* 描  述：
* 作  者：CLAY
* 版本号：v1.1
* 日  期: 2018年5月15日 2018年5月17日
* 备  注：完美实现所有功能
*         
*******************************************************************************
*/

#include "config.h"
#include "keyboard.h"
#include "led.h"
#include "time.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "eeprom.h"
#include "pcf8591.h"
#include "main.h"

enum eSysSta SysSta = E_RUN;//系统运行状态，上电默认处于运行模式
u8 T0RH, T0RL;			    //定时器T0的重载值
bit flagLedCnt = 0;			//指示灯的流转间隔标志，进行指示灯切换。
bit flag800ms = 0;			//设置状态下选中单元以0.8s为间隔进行闪烁
bit flag2s = 0;				//处于设置状态下2s保存一次流转间隔到E2中
bit flag200ms = 0;			//每隔200ms获取一次当前的亮度等级
u8 val = 0;					//获取Rb2的AD值
u8 LedLevel = 1;			//亮度等级，上电默认为1
u8 LedLevelDat[4] = {25, 50, 75, 100};//不同的亮度，越大越亮
u8 i, ModeCnt[4];			//i-E2保存数据用的！ModeCnt定义为u8类型用来对不同流转模式的间隔缩小存储
u16 ModeLedCnt[4] = {500, 500, 500, 500};//不同模式的流转间隔
u8 PWMSta = 0xFF;			//暂存P0的状态，用来PWM控制指示灯

void GetLedLevel();
void SaveLedCnt();
void TurnOverLed();
void CloseFucker();
void ConfigTimer0(u16 ms);
void ConfigTimer1();

void main()
{
#if 0//第一次写入数据时使用 (别那么着急关，下载完稍微等一下确保数据真的写入了！)
	
	//对各个模式的流转间隔缩小处理
	for(i=0; i<4; i++)
	{
		ModeCnt[i] = ModeLedCnt[i] / 100;
	}
	E2Write(ModeCnt, 0x00, 4);
#endif

#if 1
	//注意了,下面这个读取语句的位置必须放在所有的中断配置程序前面，上电先读取E2中的流转间隔，保证不会出错！
	//上电后读取E2中的数据
	E2Read(ModeCnt, 0x00, 4);
	//对各个模式的流转间隔放大处理
	for(i=0; i<4; i++)
	{
		ModeLedCnt[i]  = ModeCnt[i] * 100;
	}
	
 	EA = 1;//开启定时器总使能
	CloseFucker();
	ConfigTimer0(1);//配置定时器0-1ms
	ConfigTimer1(); //配置定时器1-10us
	
	while(1)
	{
		KeyDriver();//按键驱动扫描
		
		if(flag200ms)
		{
			flag200ms = 0;
			GetLedLevel();//每200ms获取一次亮度等级
		}
		
		if(SysSta == E_RUN)//当前系统的运行状态是运行
		{
			if(LedSta)//LED流转开关打开的前提
			{
				if(flagLedCnt)
				{
					flagLedCnt = 0;
					LedDriver();//每隔流转间隔进行指示灯的切换
				}	
			}
			
			if(flagLevel == 1)
			{
				RefreshLevel();//处于运行模式按下s4即时刷新亮度等级
			}
			else
			{
				CloseLed();//否则的话关闭数码管
			}
		}
		
		if(SysSta == E_SET)//当前的系统运行状态是设置
		{
			if(flag800ms)
			{
				flag800ms = 0;
				TurnOverLed();//设置模式下对应单元闪烁！
			}
			//处于设置状态下2s保存一次流转间隔
			if(flag2s == 1)
			{
				flag2s = 0;
				SaveLedCnt();//每隔2s存一下当前的流转间隔
			}
		}
	}
#endif
}

void GetLedLevel()//根据Rb2的AD值，获取亮度等级
{	
	val = GetADCVal(3);
	if(val <= 12)
	{
		LedLevel = 1;
	}
	else if(val <= 24)
	{
		LedLevel = 2;
	}
	else if(val <= 36)
	{
		LedLevel = 3;
	}
	else
	{
		LedLevel = 4;
	}
}

void TurnOverLed()//实现数码管闪烁的特效
{
	static bit LedCurSta=1;	
	
	if(LedCurSta)
	{	
		LedCurSta = 0;
		
		if(StaCnt == 1)
		{
			LedBuff[7] = 0xFF;
			LedBuff[6] = 0xFF;
			LedBuff[5] = 0xFF;
		}
		else if(StaCnt == 2)
		{
			LedBuff[3] = 0xFF;
			LedBuff[2] = 0xFF;
			LedBuff[1] = 0xFF;
			LedBuff[0] = 0xFF;
		}
	}
	else
	{
		LedCurSta = 1;
		
		if(StaCnt == 1)
		{
			LedBuff[7] = 0xBF;
			LedBuff[6] = LedChar[LedMode];
			LedBuff[5] = 0xBF;
		}
		else if(StaCnt == 2)
		{
			ShowNumber(LedCnt);
		}
	}
}

void SaveLedCnt()//存储流转间隔
{
	for(i=0; i<4; i++)
	{
		ModeCnt[i] = ModeLedCnt[i] / 100;
	}
	E2Write(ModeCnt, 0x00, 4);
}

void CloseFucker()//关闭恶心的蜂鸣器
{
 	P2 = (P2&0x1F) | 0xA0;
	P0 &= 0xAF;	
	P2 = P2&0x1F;
}

void ConfigTimer0(u16 ms)//配置定时器0，单位ms
{
 	u32 tmp;

	tmp = 11059200/12;
	tmp = (tmp*ms) / 1000;
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

void ConfigTimer1()		//10微秒@11.0592MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x10;		//设置定时器模式
	TL1 = 0xF7;		//设置定时初值
	TH1 = 0xFF;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;
}

void InterruptTimer0() interrupt 1
{
	static u16 tmrLedCnt=0;
	static u16 tmr800ms = 0;
	static u16 tmr2s = 0;
	static u8 tmr200ms = 0;
	
 	TH0 = T0RH;
	TL0 = T0RL;
	tmr200ms++;
	if(tmr200ms >= 200)
	{
		tmr200ms = 0;
		flag200ms = 1;
	}
	
	if(SysSta == E_SET)//这里加了一个前提，可做到在处于非设置状态下不必要的判断
	{
		tmr800ms++;
		tmr2s++;
		
		if(tmr800ms >= 800)
		{
			tmr800ms = 0;
			flag800ms = 1;
		}
		if(tmr2s >= 2000)
		{
			tmr2s = 0;
			flag2s = 1;
		}
	}
	
	if(LedSta)//这里也进行了前提判断，有个好处是再打开不会突兀的跳!(注意体会)
	{
		tmrLedCnt++;
		
		if(tmrLedCnt >= LedCnt)
		{
			tmrLedCnt = 0;
			flagLedCnt = 1;
		}
	}
	
	KeyScan();
	LedScan();
}

void InterruptTimer1() interrupt 3//定时器1可以说只是用来实现PWM控制LED了！
{	
    static u8 PWMCnt = 0;

    TH1 = 0xFF;//服气了，TH1写成了TH0。。。
    TL1 = 0xF7;
    PWMCnt++;
    PWMCnt %= 101;//确保取值在0-100之间
	
	if(PWMCnt >= LedLevelDat[LedLevel-1])
	{
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xFF;
		P2 = P2&0x1F;
	}
	else
	{
		P2 = (P2&0x1F) | 0x80;
		P0 = PWMSta;
		P2 = P2&0x1F;
	}
}
