#ifndef LED_H
#define LED_H

extern u8 code LedChar[];
extern u8 LedBuff[];

void LedScan();
void ShowNumber(u8 num);

#endif