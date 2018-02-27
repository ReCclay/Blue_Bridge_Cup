/*
*******************************************************************************
* 文件名：main.c
* 描  述：单字节读写
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：先读出0x02对应的数据，然后再进行+1后写回去！
*         
*******************************************************************************
*/

#include "config.h"
#include "uart.h"
#include "i2c.h"
#include "string.h"

u8 T0RH = 0;
u8 T0RL = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();
u8 E2ReadByte(u8 addr);
void E2WriteByte(u8 addr, u8 dat);

void main()
{
	u8 dat;
	u8 buf[10];

	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	ConfigUart(9600);

	dat = E2ReadByte(0x02);
	buf[0] = (dat/100) + '0';
	buf[1] = (dat/10%10) + '0';
	buf[2] = (dat%10) + '0';
	buf[3] = '\r';
	buf[4] = '\n';
	buf[5] = '\0';
	UartWrite(buf, strlen(buf));
		
	dat++;                     //将其数值+1
	E2WriteByte(0x02, dat); 
	while(1)
	{
		UartDriver();
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

u8 E2ReadByte(u8 addr)
{
 	u8 dat;

	I2CStart();
	I2CWrite(0x50<<1);
	I2CWrite(addr);
	I2CStart();
	I2CWrite((0x50<<1) | 0x01);	
	dat = I2CReadNAK();
	I2CStop();

	return dat;
}

void E2WriteByte(u8 addr, u8 dat)
{
 	I2CStart();
	I2CWrite(0x50<<1);
	I2CWrite(addr);
	I2CWrite(dat);
	I2CStop();
}

 	

void InterruptTimer0() interrupt 1
{
 	TH0 = T0RH;
	TL0 = T0RL;
	
	UartRxdMonitor(1);
}