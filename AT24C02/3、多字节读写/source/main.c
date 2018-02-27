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
void E2Read(u8 *buf, u8 addr, u8 len);
void E2Write(u8 *buf, u8 addr, u8 len);
void MemToStr(u8 *str, u8 *src, u8 len);

void main()
{
	u8 i;
	u8 buf[5];
	u8 str[20];

	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	ConfigUart(9600);

	E2Read(buf, 0x90, sizeof(buf));
	MemToStr(str, buf, sizeof(buf));
	UartWrite(str, strlen(str));

	for(i=0; i<sizeof(buf); i++)
	{
	 	buf[i] = buf[i]+1+i;
	}
	E2Write(buf, 0x90, sizeof(buf));

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

void MemToStr(u8 *str, u8 *src, u8 len)
{
 	u8 tmp;

	while(len--)
	{
	 	tmp = *src >> 4;
		if(tmp <= 9)
			*str++ = tmp + '0';
		else
			*str++ = tmp - 10 + 'A';
		tmp = *src & 0x0F;
		if(tmp <= 9)
			*str++ = tmp + '0';
		else
			*str++ = tmp - 10 + 'A';
		*str++ = ' ';
		src++;
	}
	*str = '\0';
}

void E2Read(u8 *buf, u8 addr, u8 len)
{
 	do{
		I2CStart();
		if(I2CWrite(0x50<<1))
		{
		 	break;
		}
		I2CStop();
	}while(1);
	I2CWrite(addr);
	I2CStart();
	I2CWrite((0x50<<1)|0x01);
	while(len > 1)
	{
	 	*buf++ = I2CReadACK();
		len--;
	}
	*buf = I2CReadNAK();
	I2CStop();
}

void E2Write(u8 *buf, u8 addr, u8 len)
{
 	while(len--)
	{
	 	do{
			I2CStart();
			if(I2CWrite(0x50<<1))
			{
			 	break;
			}
			I2CStop();
		}while(1);
		I2CWrite(addr++);
		I2CWrite(*buf++);
		I2CStop();
	}
}

void InterruptTimer0() interrupt 1
{
 	TH0 = T0RH;
	TL0 = T0RL;
	
	UartRxdMonitor(1);
}