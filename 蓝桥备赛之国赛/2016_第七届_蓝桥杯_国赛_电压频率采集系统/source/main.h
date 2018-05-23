#ifndef MAIN_H
#define MAIN_H

enum eSysSta {//分别对应 时钟界面、电压界面、频率界面、查询界面
	E_CLK, E_VOL, E_FRE, E_QRY
};

extern enum eSysSta SysSta;
extern bit flagLedOpen;
extern bit flagVolOpen;
extern u32 FreNum;
extern u32 TimNum;

extern u8 TrigType;
extern u8 TrigHour, TrigMin, TrigSec;

#endif