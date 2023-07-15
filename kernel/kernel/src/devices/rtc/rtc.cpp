#include "rtc.h"
#include "../../kernelStuff/IO/IO.h"
#include "../../kernelStuff/stuff/cstr.h"
#include "../pit/pit.h"
#include "../../osData/MStack/MStackM.h"
#define CURRENT_YEAR_STR        (__DATE__ + 7)                            // Change this each year!

// https://wiki.osdev.org/CMOS#RTC_Update_In_Progress

namespace RTC
{

    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned int year;

    int CURRENT_YEAR = 0;
    void InitRTC()
    {
        CURRENT_YEAR = to_int(CURRENT_YEAR_STR);
        LastUpdateTime = 0;
        minute = 0;
        second = 0;
        hour = 0;
        day = 1;
        month = 1;
        year = CURRENT_YEAR;

        Minute = 0;
        Second = 0;
        Hour = 0;
        Day = 1;
        Month = 1;
        Year = CURRENT_YEAR;
    }

    int century_register = 0x00;                                // Set by ACPI table parsing code if possible
    

    

    
    enum {
        cmos_address = 0x70,
        cmos_data    = 0x71
    };
    
    int get_update_in_progress_flag() {
        outb(cmos_address, 0x0A);
        return (inb(cmos_data) & 0x80);
    }
    
    unsigned char get_RTC_register(int reg) {
        outb(cmos_address, reg);
        return inb(cmos_data);
    }
    
    void read_rtc() 
    {
        AddToStack();
        unsigned char century;
        unsigned char last_second;
        unsigned char last_minute;
        unsigned char last_hour;
        unsigned char last_day;
        unsigned char last_month;
        unsigned char last_year;
        unsigned char last_century;
        unsigned char registerB;

        // Note: This uses the "read registers until you get the same values twice in a row" technique
        //       to avoid getting dodgy/inconsistent values due to RTC updates

        while (get_update_in_progress_flag());                // Make sure an update isn't in progress
        second = get_RTC_register(0x00);
        minute = get_RTC_register(0x02);
        hour = get_RTC_register(0x04);
        day = get_RTC_register(0x07);
        month = get_RTC_register(0x08);
        year = get_RTC_register(0x09);
        if(century_register != 0) 
            century = get_RTC_register(century_register);
        
    
        do 
        {
            last_second = second;
            last_minute = minute;
            last_hour = hour;
            last_day = day;
            last_month = month;
            last_year = year;
            last_century = century;

            while (get_update_in_progress_flag());           // Make sure an update isn't in progress
            second = get_RTC_register(0x00);
            minute = get_RTC_register(0x02);
            hour = get_RTC_register(0x04);
            day = get_RTC_register(0x07);
            month = get_RTC_register(0x08);
            year = get_RTC_register(0x09);
            if(century_register != 0) 
                century = get_RTC_register(century_register);
        }
        while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
                (last_day != day) || (last_month != month) || (last_year != year) ||
                (last_century != century) );
    
        registerB = get_RTC_register(0x0B);

        // Convert BCD to binary values if necessary

        if (!(registerB & 0x04)) {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);
            if(century_register != 0) 
                century = (century & 0x0F) + ((century / 16) * 10);
        }
    
        // Convert 12 hour clock to 24 hour clock if necessary
    
        if (!(registerB & 0x02) && (hour & 0x80)) {
                hour = ((hour & 0x7F) + 12) % 24;
        }
    
        // Calculate the full (4-digit) year
    
        if(century_register != 0) {
            year += century * 100;
        } else {
            year += (CURRENT_YEAR / 100) * 100;
            if(year < CURRENT_YEAR) 
            year += 100;
        }

        //UpdateTimeIfNeeded();
        RemoveFromStack();
    }



    int Second, Minute, Hour, Day, Month, Year;
    unsigned long LastUpdateTime = 0;

    void UpdateTimeIfNeeded()
    {
        if (PIT::Inited)
        {
            unsigned long now = PIT::TimeSinceBootMS();
            if (now > LastUpdateTime + 500)
            {
                LastUpdateTime = now;
                read_rtc();
            }
            else
                return;
        }
        else
            read_rtc();

        AddToStack();
        //int offset = 2;

        Second = second;
        Minute = minute;
        Hour = hour;
        Day = day;
        Month = month;
        Year = year;
        RemoveFromStack();
    }

}