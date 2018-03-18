/*
*******************************************************************************
* 文件名：ds18b20.c
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

void Delay(u8 t)//us级软件延时
{
    do {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    } while (--t);
}

bit Get18B20Ack()
{
	bit ack;

 	EA = 0;
	DS18B20_IO = 0;
	Delay(250);
	Delay(250);
	DS18B20_IO = 1;
	Delay(60);
	ack = DS18B20_IO;
	while(!DS18B20_IO);
	EA = 1;
	
	return ack;	
}

void DS18B20Write(u8 dat)
{
 	u8 mask;

	EA = 0;
	for(mask=0x01; mask!=0; mask<<=1)
	{
	 	DS18B20_IO = 0;//写肯定要占用总线！
		_nop_();
		_nop_();
		if(dat&mask)
			DS18B20_IO = 1;
		else
			DS18B20_IO = 0;
		Delay(60);
		DS18B20_IO = 1;//占用总线总得释放吧。
	}
	EA = 1;
}

u8 DS18B20Read()
{
 	u8 mask, dat=0;

	EA = 0;
	for(mask=0x01; mask!=0; mask<<=1)
	{
	 	DS18B20_IO = 0;//单片机占用总线
		_nop_();
		_nop_();
		DS18B20_IO = 1;//单片机释放总线，等待18B20操作
		_nop_();
		_nop_(); 
		if(DS18B20_IO)
			dat |= mask;
		Delay(60);
	}
	EA = 1;

	return dat;
}

bit DS18B20Start()
{
 	bit ack;

	ack = Get18B20Ack();
	if(ack == 0)
	{
	 	DS18B20Write(0xCC);	//跳过ROM
		DS18B20Write(0x44);	//启动温度转换
	}
	return ~ack;
}

bit Get18B20Temp(int *temp)
{
	bit ack;
	u8 LSB, MSB;
	
	ack = Get18B20Ack();
	if(ack == 0)
	{
		DS18B20Write(0xCC);//跳过ROM
		DS18B20Write(0xBE);//读
		LSB = DS18B20Read();
		MSB = DS18B20Read();
		*temp = ((u16)MSB << 8) + LSB;
	} 
	return ack;	
}