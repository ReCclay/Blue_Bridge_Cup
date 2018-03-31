#ifndef TIME_H
#define TIME_H

extern bit staTemp;
extern u8 MaxTemp;
extern u8 MinTemp;

void RefreshLim();
void TempMonitor();
void ChangeTemp();
void RefreshTemp();
void KeyAction(u8 keycode);

#endif