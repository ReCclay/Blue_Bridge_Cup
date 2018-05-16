#include "config.h"
#include "i2c.h"

#define VCC 48//电压是扩大10倍以后的电压

u8 GetADCValue(u8 ch)
{
	u8 val;

	I2CStart();
	if(!(I2CWrite(0x48<<1)))
	{
	 	I2CStop();
		return 0;
	}	
	I2CWrite(0x40|ch);
	I2CStart();
	I2CWrite((0x48<<1) | 0x01);
	I2CReadACK();
	val = I2CReadNAK();
	I2CStop();

	val = (val*VCC)/255;//val扩大十倍，手动加小数点

	return val;
}

void SetDACOut(u8 val)//输入也是一样的道理，默认输入扩大十倍，然后处理.
{
	val = (val*255)/VCC;

 	I2CStart();
	if(!(I2CWrite(0x48<<1)))
	{
	 	I2CStop();
		return;
	}	
	I2CWrite(0x40);
	I2CWrite(val);
	I2CStop();
}	