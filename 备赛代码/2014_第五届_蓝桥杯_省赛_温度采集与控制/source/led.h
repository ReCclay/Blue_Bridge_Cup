#ifndef LED_H
#define LED_H

u8 LedChar[];
u8 LedBuff[];

void LedScan();
void ShowNumber(u16 num);
void OpenLed(u8 x);
void CloseLed(u8 x);
void OpenRelay();
void CloseRelay();

#endif