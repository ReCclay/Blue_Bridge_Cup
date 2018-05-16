#ifndef MAIN_H
#define MAIN_H

enum eSysSta{
	E_RUN, E_SET
};

extern u8 PWMSta;
extern bit flagPWM;
extern u8 tmr1ms;
extern u8 LedLevel;
extern u16 ModeLedCnt[4];	
extern bit flagLedCnt;
extern enum eSysSta SysSta;

#endif