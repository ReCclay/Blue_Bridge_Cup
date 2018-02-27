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
#include "uart.h"
#include "main.h"

bit flagFrame = 0;
bit flagTxd = 0;
u8 cntRxd = 0;
u8 bufRxd[64];

void ConfigUart(u32 baud)
{
 	PCON &= 0x7F;
	SCON = 0x50;
	AUXR &= 0xBF;
	AUXR &= 0xFE;
	TMOD &= 0x0F;
	TMOD |= 0x20;
	TH1 = 256 - (11059200/12/32)/baud;
	TL1 = TH1;
	ET1 = 0;
	ES = 1;
	TR1 = 1;
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

u8 UartRead(u8 *buf, u8 len)
{
	u8 i;

 	if(len > cntRxd)//当指定长度大于接收长度时，更新指定长度
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

void UartRxdMonitor(u8 ms)//这个函数放到定时器0的中断中扫描
{
 	static u8 cntbkp = 0; //上一个时间间隔内接收到的数据个数
	static u8 idletmr = 0;//空闲时间累加器！

	if(cntRxd > 0)
	{
	 	if(cntbkp != cntRxd)
		{
			cntbkp = cntRxd;
			idletmr = 0;
		}
	 	else//两次数量一样，且空闲时间小于30
		{
			if(idletmr < 30)
			{
			 	idletmr += ms;
				if(idletmr >= 30)
				{
				 	flagFrame = 1;
				}
			}
		}
	}
	else
	{
	 	cntbkp = 0;
	}
}

void UartDriver()//在主函数while(1)调用
{
 	u8 len;
	u8 buf[40];

	if(flagFrame)//一帧数据接收完毕再来处理
	{
	 	flagFrame = 0;
		len = UartRead(buf, sizeof(buf));
		UartAction(buf, len);
	}
}

void InterruptUart() interrupt 4
{
 	if(RI)
	{
	 	RI = 0;
		if(cntRxd < sizeof(bufRxd))//每收到一个字节，触发一次串口中断!
		{
		 	bufRxd[cntRxd++] = SBUF;//此时只要缓冲区没有满，就把对应的数据存入缓冲区！
		}
	}
	if(TI)
	{
	 	TI = 0;
		flagTxd = 1;    
	}
}