#ifndef TIME_H
#define TIME_H

extern bit staWater;

void RefreshPriceCnt();
void RefreshWaterCnt();
void RefreshPrice();
void ADCMonitor();
void RefreshTemp();
void RefreshTime(bit sta);
void KeyAction(u8 keycode);

#endif