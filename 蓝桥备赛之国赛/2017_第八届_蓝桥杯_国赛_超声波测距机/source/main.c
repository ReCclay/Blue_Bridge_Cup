/*******************************************************************************
* 文件名：第八届国赛之超声波测距机
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2018年5月20日 2018年5月21日
* 备  注：*题目有点不全，'存完一个距离以0.2s为间隔闪烁'，并没有说持续多久，这里我按1s进行了处理
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

enum eSysSta SysSta = E_RUN;//默认系统运行状态

u8 T0RH, T0RL;//T0的装载值
bit flag200ms = 0;//200ms定时到标志
bit OpenLed1 = 0;//LED1开启标志
bit flagLed1 = 0;//LED10.2定时到标志
u8 tmrLed1 = 0;//控制间隔0.2s计数器
u8 Led1Cnt = 0;//控制持续1s计数器
u16 OprtNum = 1;//距离测量界面的操作数设置
u16 distance=10, LastDistance=0, FixDistance=0;//距离测量界面的本次距离、上次距离

void CloseFucker();//关闭蜂鸣器
void ConfigTimer0(u16 ms);//配置定时器0
void InitSonic();//初始化超声波，用的是定时器1
void SonicDriver();//超声波距离扫描
void ResetE2();//初始化E2中的0x00~0x09的这几块地址的内容
void TurnLed1();//反转LED1的状态

void main()
{
	u8 val = 0;//用来DAC输出
	
 	EA = 1;//开启总中断
	CloseFucker();//关闭蜂鸣器等无关外设
	ConfigTimer0(1);//配置定时器1ms
	InitSonic();//初始化超声波(定时器1)
	ResetE2();//初始化0x00~0x09的内容为0
		
	while(1)
	{
		KeyDriver();//矩阵按键的扫描
		
	 	if(flag200ms)//200ms定时到
		{
		 	flag200ms = 0;//清除200ms的定时到标志位
			if(SysSta == E_RUN)//判断当前系统是否处于距离测量界面
			{
				SonicDriver();//超声波扫描
			}
			
			if(distance < FixDistance)//当前距离小于修正距离，那就让DAC输出0
			{
				SetDACOut(0);
			}
			else//当前距离大于修正距离，对应DAC输出 (distance - FixDistance) * 0.02V
			{
				val = (u8)((distance-FixDistance)*0.2);
				SetDACOut(val);
			}
		}	
		if(flagLed1)//LED1 0.2s反转间隔到
		{
			flagLed1 = 0;
			TurnLed1();//进行LED1的反转
		}
	}
}

void TurnLed1()
{
	static bit Led1Sta = 0;//控制LED反转状态的静态变量
	
	if(Led1Sta == 0)//当前灯是灭
	{
		Led1Sta = 1;//更新状态位
		P2 = (P2&0x1F) | 0x80;//打开LED1	
		P0 = 0xFE;
		P2 = P2&0x1F;
	}
	else//当前灯是亮
	{
		Led1Sta = 0;//更新状态位
		P2 = (P2&0x1F) | 0x80;//关闭LED1
		P0 = 0xFF;
		P2 = P2&0x1F;
	}
	
	Led1Cnt++;//1s间隔计数器
	if(Led1Cnt >= 6)//注意了这样才是1s
	{
		Led1Cnt = 0;//清除控制间隔时间的计数变量，以便下次再计数
		OpenLed1 = 0;//关闭LED1开关
		tmrLed1 = 0;//清除0.2ms的定时器计数
		
		P2 = (P2&0x1F) | 0x80;//确保关闭LED1
		P0 = 0xFF;
		P2 = P2&0x1F;
	}
	
}

void ResetE2()//初始化E2中的数据从0x00~0x09
{
	u8 i, dat=0;
	
	for(i=0; i<10; i++)
	{
		E2Write(&dat, i, 1);
	}
}

void CloseFucker()
{
 	P2 = (P2&0x1F) | 0xA0;
	P0 &= 0xAF;	
	P2 = P2&0x1F;
}

void ConfigTimer0(u16 ms)
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

void Delay13us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}


void InitSonic()
{
	TMOD &= 0x0F;
	TMOD |= 0x10;
	TF1 = 0;
	TR1 = 0;
}

void SendWave()
{
	u8 i;
	
	while(i--)
	{
		Sonic_Txd = 1;
		Delay13us();
		Sonic_Txd = 0;
		Delay13us();
	}
}

void SonicDriver()
{
	u16 time;
	static u16 Result=0;//为什么Result非得是静态或者全局变量，局部变量就不行了？
	
	TH1 = 0;
	TL1 = 0;
	SendWave();//连续发送8个40K的方波
	TR1 = 1;
	while((!TF1) && (Sonic_Rxd));//当TF未溢出并且RXD处于高电平期间就让计数器一直计数
	TR1 = 0;
	
	LedBuff[7] = LedChar[0];//对应测量界面的数码管固定显示
	LedBuff[6] = 0xFF;
	
	if(TF1 == 1)
	{
		TF1 = 0;
		distance = LastDistance;//溢出了就用上次的测量值
	}
	else
	{
		time = (TH1 << 8) + TL1;//未溢出就用时间换算成距离，乘以声速除以2(注意单位)
		distance = (u16)(time*0.017);
	}
	LedBuff[0] = LedChar[distance%10];//对应单元显示当前距离
	LedBuff[1] = LedChar[distance/10%10];
	LedBuff[2] = LedChar[distance/100%10];
	
	if(OprtNum == 0)
	{
		Result = LastDistance;//操作数是0的情况时，中间显示上次的测量距离
	}
	else
	{
		Result = LastDistance + distance;//操作数是1的情况时，中间显示上次的测量距离+本次测量距离
	}
	LedBuff[3] = LedChar[Result%10];//对应单元显示的对应距离
	LedBuff[4] = LedChar[Result/10%10];
	LedBuff[5] = LedChar[Result/100%10];
	
	LastDistance = distance;//更新上次的测量值
}

void InterruptTimer0() interrupt 1
{
	static u8 tmr200ms = 0;	
	
 	TH0 = T0RH;
	TL0 = T0RL;
	tmr200ms++;
	
	if(OpenLed1)//判断是否LED1开关打开
	{
		tmrLed1++;
		if(tmrLed1 >= 200)//0.2s的间隔
		{
			tmrLed1 = 0;
			flagLed1 = 1;
		}
	}

	if(tmr200ms >= 200)
	{
	 	tmr200ms = 0;
		flag200ms = 1;
	}
	
	KeyScan();//按键扫描
	LedScan();//数码管刷新
}