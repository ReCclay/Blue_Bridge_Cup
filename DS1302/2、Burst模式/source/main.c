/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：Burst在Single的基础上稍作修改！
*         Burst读 - 0xBF, Burst写 - 0xBE
*******************************************************************************
*/

#include "config.h"
#include "ds1302.h"
#include "lcd1602.h"

u8 T0RH = 0;
u8 T0RL = 0;
bit flag200ms = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();

void main()
{
	u8 i;
	u8 psec = 0xAA;
	u8 time[8];
	u8 str[12];

	
	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();
	InitLcd1602();
	

	while(1)
	{
		if(flag200ms)
		{
		 	flag200ms = 0;
			for(i=0; i<7; i++)
			{
			 	time[i] = DS1302SingleRead(i);
			}
			if(psec != time[0])
			{
			    str[0] = '2';  //添加年份的高2位：20
                str[1] = '0';
                str[2] = (time[6] >> 4) + '0';  //“年”高位数字转换为ASCII码
                str[3] = (time[6]&0x0F) + '0';  //“年”低位数字转换为ASCII码
                str[4] = '-';  //添加日期分隔符
                str[5] = (time[4] >> 4) + '0';  //“月”
                str[6] = (time[4]&0x0F) + '0';
                str[7] = '-';
                str[8] = (time[3] >> 4) + '0';  //“日”
                str[9] = (time[3]&0x0F) + '0';
                str[10] = '\0';
                LcdShowStr(0, 0, str);  //显示到液晶的第一行
                
                str[0] = (time[5]&0x0F) + '0';  //“星期”
                str[1] = '\0';
                LcdShowStr(11, 0, "week");
                LcdShowStr(15, 0, str);  //显示到液晶的第一行
                
                str[0] = (time[2] >> 4) + '0';  //“时”
                str[1] = (time[2]&0x0F) + '0';
                str[2] = ':';  //添加时间分隔符
                str[3] = (time[1] >> 4) + '0';  //“分”
                str[4] = (time[1]&0x0F) + '0';
                str[5] = ':';
                str[6] = (time[0] >> 4) + '0';  //“秒”
                str[7] = (time[0]&0x0F) + '0';
                str[8] = '\0';
                LcdShowStr(4, 1, str);  //显示到液晶的第二行
                
                psec = time[0];  //用当前值更新上次秒数
			}
		}		
	}
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
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH; 
    TL0 = T0RL;
    tmr200ms++;
    if (tmr200ms >= 200) 
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
}
