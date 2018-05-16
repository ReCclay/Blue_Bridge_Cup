#define _DIGITAL_C
#include "digital.h"
#include "config.h"


unsigned char code LedChar[] = { 
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = { 	//666  ”8个小灯何尝不是相当一个数码管呢？！！！“
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// 显示到数码管上，num-待显示数字 
void ShowNumber(unsigned long num)
{
    signed char i;
    unsigned char buf[6];
    
    for (i=0; i<6; i++)  
    {
        buf[i] = num % 10;
        num = num / 10;
    }
    for (i=5; i>=1; i--)  
    {
        if (buf[i] == 0 && i != 2)
            LedBuff[i] = 0xFF;
		else if(i == 2)
			LedBuff[i] = 0xBF; //0xBF就是字符'-'.
        else
            break;
    }
    for ( ; i>=0; i--)   
    {
        LedBuff[i] = LedChar[buf[i]];
    }
	if(LedBuff[1] == 0xFF)
		LedBuff[1] = LedChar[0];	
}

//单个数码管位索引显示功能(从右到左对应0～5)	外加6对应的是8个小灯
unsigned char ShowLedNumber(unsigned int index, unsigned int num, unsigned int point)
{
    LedBuff[index] = LedChar[num];  //输入数字转换为数码管字符0～F
    if (point != 0)
    {
        LedBuff[index] &= 0x7F;  //point不为0时点亮当前位的小数点
    }
	return num;
}

//void Light()
//{
//	unsigned long i;
// 	LedBuff[6] = 0x00;
//	for(i=0; i<300000; i++);
//	LedBuff[6] = 0xFF;
//	for(i=0; i<300000; i++); 	
//}


void LedScan()
{
    static unsigned char i = 0;  
    
    P0 = 0xFF;             
    P1 = (P1 & 0xF8) | i;  
    P0 = LedBuff[i];       
    if (i < 6) //注意注意当要把数码管和小灯放到一起时，这里得是6!!!           
        i++;
    else
        i = 0;
}

