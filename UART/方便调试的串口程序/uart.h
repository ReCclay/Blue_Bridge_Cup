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

extern void UartInit();
extern void SendData(u8 dat);
extern void SendString(u8 *str);

#endif