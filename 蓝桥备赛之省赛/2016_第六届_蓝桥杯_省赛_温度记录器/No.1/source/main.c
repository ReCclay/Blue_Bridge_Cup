#define _MAIN_C
#include "digital.h"
#include "keyboard.h"
#include "main.h"
#include "config.h"
#include "DS1302.h"
#include "DS18B20.h"

bit flagCountDown=0;
bit flag200ms = 0;
struct sTime pdata CurTime;
unsigned char CountDown=0;//采集间隔时间
unsigned char T0RH = 0;  
unsigned char T0RL = 0; 
unsigned char T1RH = 0;  
unsigned char T1RL = 0;
unsigned char StaSys=1; //设置一个变量当做系统状态
unsigned char Temp[10] = 0;
unsigned char index=0;//用来温度的索引


void RefreshTime();
void ConfigTimer1(unsigned int ms);
void ConfigTimer0(unsigned int ms);
void TransDisTemp(); 
void CollTemp();
unsigned char TransTemp();


void main()
{
	unsigned long i =0;
    EA = 1;       
    ENLED = 0;    
    ADDR3 = 1;
	InitDS1302();
	Start18B20();

	ConfigTimer0(1);//T0用于led和key扫描
	ConfigTimer1(50);//T1用于计时采集温度
	  
	//设定数码管初始状态
	ShowLedNumber(0,1,0);
	ShowLedNumber(1,0,0);
	LedBuff[2] = 0xBF;

    	
    while (1)
    {	
		KeyDriver();
		if(StaSys == 2)
		{
		 	if(flag200ms==1)
			{
				flag200ms = 0;
				RefreshTime();
			}
			if(flagCountDown==1)
			{
				flagCountDown=0;
				CollTemp();
			}	
		}
		else if(StaSys == 3)
			TransDisTemp();				
    }
}

void KeyAction(unsigned char keycode)
{
	if(keycode == 0x34)//是否按下了S4键（切换温度）
	{
		CountDown = TransTemp();			
	}
	else if(keycode == 0x35)
	{
		if(CountDown == 0)
			CountDown = 1;	
		StaSys = 2;
		RefreshTime();
	}
	else if(keycode == 0x36)
	{
		index++;
		TransDisTemp();
		if(index==10)
			index = 0;
	}
	else if(keycode == 0x37)
	{
	 	StaSys = 1;
		CountDown = TransTemp();
	}
		
}

void CollTemp()
{
	static unsigned char i=0;
 	int temp;
	if(i<10)
	{
		if(Get18B20Temp(&temp))
		{
			temp >>= 4;//舍弃小数位
			Temp[i]=temp;
			i++;
		}
	}
	else
	 	StaSys=3;

	Start18B20();		
}

unsigned char TransTemp()
{
	static unsigned char i=0;
	unsigned char buff[4]={1,5,30,60};

	switch(i)
	{
	 	case 0: i++; ShowNumber(buff[i]); break;
		case 1: i++; ShowNumber(buff[i]); break;
		case 2: i++; ShowNumber(buff[i]); break;	
		case 3: i=0; ShowNumber(buff[i]); break;		
	}
	return buff[i];
}

void TransDisTemp()
{
	
	LedBuff[4]=0xFF;
	LedBuff[3]=0xFF;
	LedBuff[2]=0xBF;

	ShowLedNumber(5,index,0);
	LedBuff[0]=LedChar[Temp[index]%10];
	LedBuff[1]=LedChar[(Temp[index]/10)%10];
			
}

void RefreshTime()
{
    GetRealTime(&CurTime);                  //获取当前日期时间
    ShowLedNumber(5, CurTime.hour>>4, 0);   //时
    ShowLedNumber(4, CurTime.hour&0xF,1);
    ShowLedNumber(3, CurTime.min>>4,  0);   //分
    ShowLedNumber(2, CurTime.min&0xF, 1);
    ShowLedNumber(1, CurTime.sec>>4,  0);   //秒
    ShowLedNumber(0, CurTime.sec&0xF, 0);
}


void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  
    
    tmp = 11059200 / 12;    
    tmp = (tmp * ms) / 1000;  
    tmp = 65536 - tmp;       
    tmp = tmp + 18;          
    T0RH = (unsigned char)(tmp>>8); 
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   
    TMOD |= 0x01;   
    TH0 = T0RH;    
    TL0 = T0RL;
    ET0 = 1;        
    TR0 = 1;      
}

void ConfigTimer1(unsigned int ms)
{
    unsigned long tmp;  
    
    tmp = 11059200 / 12;    
    tmp = (tmp * ms) / 1000;  
    tmp = 65536 - tmp;       
    tmp = tmp + 18;          
    T1RH = (unsigned char)(tmp>>8); 
    T1RL = (unsigned char)tmp;
    TMOD &= 0x0F;   
    TMOD |= 0x10;   
    TH1 = T1RH;    
    TL1 = T1RL;
    ET1 = 1;        
    TR1 = 1;      
}

void InterruptTimer0() interrupt 1//每200ms进行一次时钟刷新
{
    static unsigned char tmr200ms = 0;  
    
    TH0 = T0RH;   
    TL0 = T0RL;     
    LedScan();   
    KeyScan();  

	tmr200ms++;
    if (tmr200ms>=200)  
    {
		flag200ms = 1;
        tmr200ms=0;
    }
}

void InterruptTimer1() interrupt 3 //定时器一用来隔CountDown来采集一次温度
{
    static unsigned char tmr50ms = 0;
    static unsigned char tmr1s = 0;

    TH1 = T1RH;   
    TL1 = T1RL;      

	tmr50ms++;
	if(tmr50ms >=20)
	{
		tmr50ms = 0;
		tmr1s++;		
		if(tmr1s>=CountDown)
		{
			tmr1s=0;
			flagCountDown=1;		
	 	}
	}
}
