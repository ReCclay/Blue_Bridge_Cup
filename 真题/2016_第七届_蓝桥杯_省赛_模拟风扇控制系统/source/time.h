#ifndef TIME_H
#define TIME_H

extern u8 CountTime;
extern bit staTemp;

void RefreshTemp();
void KeyAction(u8 keycode);
void RefreshMode(u8 mode);
void LedTurn(u8 x);

#endif