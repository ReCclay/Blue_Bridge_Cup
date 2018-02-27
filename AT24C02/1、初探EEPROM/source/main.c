/*
*******************************************************************************
* 文件名：main.c
* 描  述：初探EEPROM
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#include "config.h"
#include "uart.h"
#include "i2c.h"

u8 T0RH = 0;
u8 T0RL = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();

void main()
{
	bit ack;
	u8 buf[3];

	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	ConfigUart(9600);


	while(1)
	{
		UartDriver();

		ack = I2CAddressing(0x50);
		buf[0] = (u8)ack + '0';
		buf[1] = '\r';
		buf[2] = '\n';
		UartWrite(buf, sizeof(buf));
	}
}

void CloseFucker()
{
 	P2 = (P2 & 0x1F) | 0xA0;
	P0 = P0 & 0xAF;
	P2 = 0x00;
}

void ConfigTimer0(u16 ms)
{
 	u32 tmp;
	
	tmp = 11059200/12;
	tmp = (tmp*ms)/1000;
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

void UartAction(u8 *buf, u8 len)
{
	buf[len++] = '\r';
	buf[len++] = '\n';
	UartWrite(buf, len);	
}

void InterruptTimer0() interrupt 1
{
 	TH0 = T0RH;
	TL0 = T0RL;
	
	UartRxdMonitor(1);
}