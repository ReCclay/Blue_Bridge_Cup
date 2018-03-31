#include "config.h"

void Delay(u8 us)
{
 	do{
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}while(--us);
}

bit Get18B20Ack()
{
	bit ack;
	
	DS18B20_IO = 0;
	Delay(250);
	Delay(250);
	DS18B20_IO = 1;
	Delay(60);
	ack = DS18B20_IO;
	while(!DS18B20_IO);
	
	return ack; 	
}

void DS18B20Write(u8 dat)
{
 	u8 mask;

	for(mask=0x01; mask!=0; mask<<=1)
	{
		DS18B20_IO = 0;
		Delay(2);
	 	if(dat&mask)
		{
		 	DS18B20_IO = 1;
		}
		else
		{
		 	DS18B20_IO = 0;
		}
		Delay(60);
		DS18B20_IO = 1;
	}
}

u8 DS18B20Read()
{
 	u8 mask, dat=0;
	
	for(mask=0x01; mask!=0; mask<<=1)
	{
		DS18B20_IO = 0;
		Delay(2);
		DS18B20_IO = 1;
		Delay(2);
	 	if(DS18B20_IO)
		{
		 	dat |= mask;
		}
		Delay(60);
	}
	
	return dat;	
}

bit Start18B20()
{
 	bit ack;

	ack = Get18B20Ack();
	if(ack == 0)
	{
	 	DS18B20Write(0xCC);
		DS18B20Write(0x44);	
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
	 	DS18B20Write(0xCC);
		DS18B20Write(0xBE);
		LSB = DS18B20Read();
		MSB = DS18B20Read();
		*temp = ((u16)MSB<<8) + LSB;
	}

	return ~ack;
} 