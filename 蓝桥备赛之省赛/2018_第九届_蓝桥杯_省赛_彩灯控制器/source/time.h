#ifndef TIME_H
#define TIME_H

extern u16 LedCnt;
extern u8 LedMode;
extern u16 LedCnt;
extern bit LedSta;
extern u8 StaCnt;

void LedDriver();
void RefreshTime(bit sta);
void RefreshTemp();
void KeyAction(u8 keycode);
void RefreshSet();
void RefreshLevel();
void CloseLed();

#endif