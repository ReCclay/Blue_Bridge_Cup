#ifndef _DS1302_H
#define _DS1302_H

struct sTime {  //日期时间结构
    uint16 year; //年
    uint8 mon;   //月
    uint8 day;   //日
    uint8 hour;  //时
    uint8 min;   //分
    uint8 sec;   //秒
    uint8 week;  //星期
};

#ifndef _DS1302_C

#endif

void InitDS1302();
void GetRealTime(struct sTime *time);
void SetRealTime(struct sTime *time);
#endif
