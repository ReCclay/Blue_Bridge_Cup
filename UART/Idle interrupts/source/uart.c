/*
*******************************************************************************
* 文件名：uart.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#include "config.h"
#include "main.h"

bit flagTxd = 0;
bit flagFrame = 0;
u8 cntRxd = 0;
u8 bufRxd[64];

void ConfigUart(u32 baud)
{
 	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TH1 = 256 - (11059200/12/32)/baud;
	TL1 = TH1;
	ET1 = 0;
	ES = 1;
	TR1 = 1;	
}

u8 UartRead(u8 *buf, u8 len)
{
	u8 i;

	if(len > cntRxd)
	{
	 	len = cntRxd;
	}	
	for(i=0; i<len; i++)
	{
	 	*buf++ = bufRxd[i];
	}
	cntRxd = 0;

	return len;
}

void UartWrite(u8 *buf, u8 len)
{
	while(len--)
	{
	 	flagTxd = 0;
		SBUF = *buf++;
		while(!flagTxd);
	} 	
}

void UartDriver()
{
	u8 buff[40];
	u8 len;

	if(flagFrame)
	{
	 	flagFrame = 0;
		len = UartRead(buff, sizeof(buff));
		UartAction(buff, len);
	}
}

void UartRxdMonitor(u8 ms)
{
 	static u8 cntbkp = 0;
	static u8 idletmr = 0;

	if(cntRxd > 0)
	{
	 	if(cntRxd != cntbkp)
		{
		 	cntbkp = cntRxd;
			idletmr = 0;
		}
		else
		{
		 	if(idletmr < 30)
			{
			 	idletmr += ms;
			}
			if(idletmr >= 30)
			{
				idletmr = 0;
			 	flagFrame = 1;
			}
		}
	}
	else
	{
	 	cntbkp = 0;
	}
}

void InterruptUart() interrupt 4
{
 	if(RI)
	{
	 	RI = 0;
		if(cntRxd < sizeof(bufRxd))
		{
		 	bufRxd[cntRxd++] = SBUF;
		}
	}
	if(TI)
	{
	 	TI = 0;
		flagTxd = 1;
	}
}