#ifndef TIME_H
#define TIME_H

extern bit flagLED2;
extern bit SetSta;
extern u8 TempScale;

void TempMonitor();
void RefreshTemp();
void KeyAction(u8 keycode);

#endif