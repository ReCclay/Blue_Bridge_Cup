/*
*******************************************************************************
* 文件名：uart.h
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 
* 备  注：
*         
*******************************************************************************
*/

#ifndef UART_H
#define UART_H

void ConfigUart(u32 baud);
void UartDriver();
void UartRxdMonitor(u8 ms);
void UartWrite(u8 *buf, u8 len); 

#endif