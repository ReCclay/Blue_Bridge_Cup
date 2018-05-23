#ifndef TIME_H
#define TIME_H

void RefreshTime(bit sta);
void RefreshTemp();
void KeyAction(u8 keycode);
void RefreshVol();
void RefreshVolSet();
void RefreshFre(bit sta);

extern u8 SetClkCursor;
extern u8 SetClkIndex;
extern u8 SetVolIndex;
extern u8 SetVolCursor;

extern u8 VolUpLim;
extern u8 VolDoLim;
extern bit FreMode;

#endif