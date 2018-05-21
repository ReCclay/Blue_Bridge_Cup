#ifndef MAIN_H
#define MAIN_H

enum eSysSta {//对应三种系统状态：距离测量、存储显示、修正设置
	E_RUN, E_VIEW, E_SET
}; 

extern enum eSysSta SysSta;
extern u16 distance, LastDistance, FixDistance;
extern bit OpenLed1;
extern u8 Led1Cnt;

void ResetE2();

#endif