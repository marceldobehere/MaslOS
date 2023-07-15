#pragma once

namespace RTC
{
    void InitRTC();

    // extern unsigned char second;
    // extern unsigned char minute;
    // extern unsigned char hour;
    // extern unsigned char day;
    // extern unsigned char month;
    // extern unsigned int year;
    // extern int CURRENT_YEAR;

    extern int Second, Minute, Hour, Day, Month, Year;
    extern unsigned long LastUpdateTime;

    void read_rtc();
    void UpdateTimeIfNeeded();

}