/*
*******************************************************************************
* 文件名：i2c.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#include "config.h"
#include "intrins.h"
#include "uart.h"

#define I2CDelay() Delay5us()

void Delay5us()		//@11.0592MHz 这个延时是STC软件生成的
{
	unsigned char i;

	_nop_();
	i = 11;
	while (--i);
}
 

void I2CStart()//起始信号SCL/SDA高电平期间，先拉低SDA，再拉低SCL。
{
 	I2C_SCL = 1;
	I2C_SDA = 1;
	I2CDelay();
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 0;
	I2CDelay();
}

void I2CStop()//停止信号SCL/SDA低电平期间，先拉高SCL，再拉低SDA。
{
	I2C_SCL = 0;
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 1;
	I2CDelay();
	I2C_SDA = 1;
	I2CDelay();
}

bit I2CWrite(u8 dat)//写数据
{
	bit ack;
 	u8 mask;//掩码的下面这个写法务必得get到！

	for(mask=0x80; mask!=0; mask>>=1)//I2C高位在先，故mask初值0x80
	{
	 	if((mask&dat) == 0)
			I2C_SDA = 0;
		else
			I2C_SDA = 1;
		I2CDelay();
		I2C_SCL = 1;
		I2CDelay();
		I2C_SCL = 0;
		I2CDelay();
	}					 //把发送器(单片机)把一个字节数据位写入完毕，接下来该接收器(24C02)回应一个ACK
	I2C_SDA = 1;		 //主机释放SDA(显然上面一直都是发送器进行数据控制写入到接收器)，以便从机响应对应的ACK。
	I2CDelay();
	I2C_SCL = 1;		 //拉高SCL准备读SDA上对应的ACK
	ack = I2C_SDA;		 //读取ACK
	I2CDelay();
	I2C_SCL = 0;		 //读取完毕，拉低SCL,以便后面数据再写入
	I2CDelay();

	return (~ack);
}

u8 I2CReadACK()
{
	u8 mask;
	u8 dat;

	I2C_SDA = 1;
	I2CDelay();
	for(mask=0x80; mask!=0; mask>>=1)
	{
		I2C_SCL = 1;
		I2CDelay();
		if(I2C_SDA == 1)
		{
		 	dat |= mask;
		}
		else
		{
		 	dat &= ~mask;
		}
		I2CDelay();
		I2C_SCL = 0;
		I2CDelay();
	}
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 1;
	I2CDelay();
	I2C_SCL = 0;
	I2CDelay();

	return dat;
}

u8 I2CReadNAK()
{
 	u8 mask;
	u8 dat;

	I2C_SDA = 1;
	I2CDelay();
	for(mask=0x80; mask!=0; mask>>=1)
	{
	 	I2C_SCL	= 1;
		I2CDelay();
		if(I2C_SDA == 1)
			dat |= mask;
		else
			dat &= ~mask;
		I2CDelay();
		I2C_SCL = 0;
		I2CDelay();			
	}
	I2C_SDA = 1;
	I2CDelay();
	I2C_SCL = 1;
	I2CDelay();
	I2C_SCL = 0;
	I2CDelay();

	return dat;
}

 	