#ifndef DS1302_H
#define DS1302_H

struct sTime {
	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 sec;
	u8 week;
};

void InitDS1302();
void GetRealTime(struct sTime* time);
void SetRealTime(struct sTime* time);

#endif