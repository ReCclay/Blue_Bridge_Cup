#include "config.h"

#define I2CDelay() Delay5us()

void Delay5us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	i = 11;
	while (--i);
}

void I2CStart()
{
	I2C_SCL = 1;
 	I2C_SDA = 1;
	I2CDelay();
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 0;
	I2CDelay();
}

void I2CStop()
{
 	I2C_SCL = 0;
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 1;
	I2CDelay();
	I2C_SDA = 1;
	I2CDelay();
}

bit I2CWrite(u8 dat)
{
 	bit ack;
	u8 mask;

	for(mask=0x80; mask!=0; mask>>=1)
	{
	 	if(dat&mask)
		{
		 	I2C_SDA = 1;	
		}
		else
		{
		 	I2C_SDA = 0;
		}
		I2CDelay();
		I2C_SCL = 1;
		I2CDelay();
		I2C_SCL = 0;
		I2CDelay();
	}
	I2C_SDA = 1;//释放总线
	I2CDelay();
	I2C_SCL = 1;
	ack = I2C_SDA;
	I2CDelay();
	I2C_SCL = 0;

	return ~ack; //注意反逻辑输出
} 

u8 I2CReadACK()
{
	u8 mask, dat=0;

	I2C_SDA = 1;
	for(mask=0x80; mask!=0; mask>>=1)
	{
		I2C_SCL = 1;
		I2CDelay();
	 	if(I2C_SDA)
		{
		 	dat |= mask;
		}
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
	u8 mask, dat=0;

	I2C_SDA = 1;
	for(mask=0x80; mask!=0; mask>>=1)
	{
		I2C_SCL = 1;
		I2CDelay();
	 	if(I2C_SDA)
		{
		 	dat |= mask;
		}
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