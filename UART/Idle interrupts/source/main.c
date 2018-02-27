/*
*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：空闲中断配合液晶的练习
*         
*******************************************************************************
*/

#include "config.h"
#include "uart.c"
#include "lcd1602.h"

u8 T0RH, T0RL;
bit flagLedOn = 0;

void CloseFucker();
void OpenLed();
void CloseLed();
void ConfigTimer0(u16 ms);


void main()
{
 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	ConfigUart(9600);
	InitLcd1602();

 	while(1)
	{
	 	UartDriver();

	}
}

void CloseFucker()
{
 	P2 = (P2 & 0x1F) | 0xA0;
	P0 = P0 & 0xAF;
	P2 = P2 & 0x1F;
}

bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
    while (len--)
    {
        if (*ptr1++ != *ptr2++)  //遇到不相等数据时即刻返回0
        {
            return 0;
        }
    }
    return 1;  //比较完全部长度数据都相等则返回1
}

void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char code cmd0[] = "buzz on";   
    unsigned char code cmd1[] = "buzz off";  
    unsigned char code cmd2[] = "showstr ";  
    unsigned char code cmdLen[] = {         
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
    };
    unsigned char code *cmdPtr[] = {         
        &cmd0[0],  &cmd1[0],  &cmd2[0],
    };

    for (i=0; i<sizeof(cmdLen); i++)  
    {
        if (len >= cmdLen[i])  
        {
            if (CmpMemory(buf, cmdPtr[i], cmdLen[i]))  
            {
                break;
            }
        }
    }
    switch (i)  
    {
        case 0:

            break;
        case 1:

            break;
        case 2:
            buf[len] = '\0';  
            LcdShowStr(0, 0, buf+cmdLen[2]);  
            i = len - cmdLen[2];             
            if (i < 16) 
            {
                LcdAreaClear(i, 0, 16-i);
            }
            break;
        default:  
            UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
            return;
    }
    buf[len++] = '\r'; 
    buf[len++] = '\n';  
    UartWrite(buf, len);
}

void ConfigTimer0(u16 ms)
{
 	u32 tmp;

	tmp = 11059200 / 12;
	tmp = (tmp * ms) / 1000;
	tmp = 65536 - tmp;
	T0RH = (u8)(tmp >> 8);
	T0RL = (u8)tmp;
	TMOD &= 0xF0;
	TMOD |= 0x01;
	TH0 = T0RH;
	TL0 = T0RL;
	ET0 = 1;
	TR0 = 1;
}

void InterruptTimer0() interrupt 1
{
 	TH0 = T0RH;
	TL0 = T0RL;
	
	UartRxdMonitor(1);	
}

