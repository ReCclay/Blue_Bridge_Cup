#define  _DS18B20_C
#include "config.h"
#include "DS18B20.h"

void DelayX10us(uint8 t)
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
    IO_18B20 = 0;     
    DelayX10us(50);
    IO_18B20 = 1;
    DelayX10us(6);   
    ack = IO_18B20; 
    while(!IO_18B20); 
    EA = 1;  
    
    return ack;
}

void Write18B20(uint8 dat)
{
    uint8 mask;
    
    EA = 0;  
    for (mask=0x01; mask!=0; mask<<=1)  
    {
        IO_18B20 = 0;       
        _nop_();
        _nop_();
        if ((mask&dat) == 0) 
            IO_18B20 = 0;
        else
            IO_18B20 = 1;
        DelayX10us(6);        
        IO_18B20 = 1;         
    }
    EA = 1;  
}

uint8 Read18B20()
{
    uint8 dat;
    uint8 mask;
    
    EA = 0;  
    for (mask=0x01; mask!=0; mask<<=1) 
    {
        IO_18B20 = 0;        
        _nop_();
        _nop_();
        IO_18B20 = 1;        
        _nop_();            
        _nop_();
        if (!IO_18B20)       
            dat &= ~mask;
        else
            dat |= mask;
        DelayX10us(6);      
    }
    EA = 1;  

    return dat;
}

bit Start18B20()
{
    bit ack;
    
    ack = Get18B20Ack();  
    if (ack == 0)         
    {
        Write18B20(0xCC); 
        Write18B20(0x44); 
    }
    return ~ack;   
}

bit Get18B20Temp(int16 *temp)
{
    bit ack;
    uint8 LSB, MSB; 
    
    ack = Get18B20Ack();  
    if (ack == 0)           
    {
        Write18B20(0xCC);   
        Write18B20(0xBE);   
        LSB = Read18B20(); 
        MSB = Read18B20(); 
        *temp = ((int16)MSB << 8) + LSB; 
    }
	return ~ack;  
}
