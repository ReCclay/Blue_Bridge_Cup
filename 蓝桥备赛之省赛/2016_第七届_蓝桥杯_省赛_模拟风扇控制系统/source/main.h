#ifndef MAIN_H
#define MAIN_H

enum estaSys{
	E_SLEEP, E_NATURE, E_NORMAL
};

extern enum estaSys staSys;
void ConfigPWM(u8 dc);

#endif