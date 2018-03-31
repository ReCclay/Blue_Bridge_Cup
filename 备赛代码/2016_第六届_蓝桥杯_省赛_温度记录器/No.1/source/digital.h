#ifndef _DIGITAL_H
#define _DIGITAL_H

#ifndef _DIGITAL_C
extern unsigned char LedBuff[7];
extern unsigned char code LedChar[16];
#endif

unsigned char ShowLedNumber(unsigned int index, unsigned int num, unsigned int point);
void ShowNumber(unsigned long num);
void LedScan();
//void Light();

#endif
