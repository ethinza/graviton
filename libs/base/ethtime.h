///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Copyright (c) 2018 Ethinza Inc., All rights reserved                      //
//                                                                           //
// This file contains TRADE SECRETs of ETHINZA INC. and is CONFIDENTIAL.     //
// Without written consent from ETHINZA INC., it is prohibited to disclose   //
// or reproduce its contents, or to manufacture, use or sell it in whole or  //
// part. Any reproduction of this file without written consent of ETHINZA    //
// INC. is a violation of the copyright laws and is subject to civil         //
// liability and criminal prosecution.                                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <ethinza.h>

#include <ctime>

// Day definition
enum class WeekDay
{
    Sunday = 0,
    Monday = 1,
    Tuesday = 2,
    Wednesday = 3,
    Thursday = 4,
    Friday = 5,
    Saturday = 6,

    LastWeekDay
};

/**
 * @class EthTime
 * @author Chirag
 * @date 09/12/2018
 * @file ethtime.h
 * @brief Ethinza type time class
 */
class EthTime
{
private:

    // time structure
    typedef union stime_
    {
        u64_t       u64;

        struct
        {
            u32_t   date;
            u32_t   time;
        } calendar;

        struct
        {
            u8_t    year;
            u8_t    month;
            u8_t    day;
            u8_t    wday;

            u8_t    hour;
            u8_t    minute;
            u8_t    second;
            u8_t    zone;
        } time;
    } stime_t;
    
    stime_t m_time;

    // Converts to tm type
    void ToTm(struct tm* ret)
    {
        ret->tm_year    = m_time.time.year + 100;
        ret->tm_mon     = m_time.time.month - 1;
        ret->tm_mday    = m_time.time.day;
        ret->tm_hour    = m_time.time.hour;
        ret->tm_min     = m_time.time.minute;
        ret->tm_sec     = m_time.time.second;
        ret->tm_wday    = m_time.time.wday;
    }

    // Converts from tm type
    void FromTm(struct tm* tms)
    {
        m_time.time.year    = tms->tm_year - 100;
        m_time.time.month   = tms->tm_mon + 1;
        m_time.time.day     = tms->tm_mday;
        m_time.time.hour    = tms->tm_hour;
        m_time.time.minute  = tms->tm_min;
        m_time.time.second  = tms->tm_sec;
        m_time.time.wday    = (tms->tm_wday >= static_cast<int>(WeekDay::LastWeekDay)) ? 0 : tms->tm_wday;
    }

public:

    // default ctor
    EthTime()
    {
        m_time.u64 = 0;
    }

    /**
     * @brief Takes linux raw time to create object
     * @param rawTime
     */
    EthTime(struct tm* rawTime)
    {
        FromTm(rawTime);
    }

    // dtor
    ~EthTime()
    {
    }

    /**
     * @brief copy ctor
     * @param right
     */
    EthTime(const EthTime& right)
    {
        if (&right == this) return;
        
        m_time.u64 = right.m_time.u64;
    }

    /**
     * @brief assignment operator
     * @param right
     */
    EthTime& operator = (const EthTime& right)
    {
        if (&right == this) return *this;

        m_time.u64 = right.m_time.u64;
    }

    // Gets string for timezone
    String ZoneToString(void)
    {
        return "-0500";
    }

    /**
     * @brief converts time object in string 
     * @return 
     */
    String ToString(void)
    {
        char tempString[static_cast<int>(Constant::TimeStringLength) + 1] = {0};

        snprintf(tempString, static_cast<int>(Constant::TimeStringLength),
                    "%02d/%02d/%02d %02d:%02d:%02d",
                    m_time.time.year, m_time.time.month, m_time.time.day,
                    m_time.time.hour, m_time.time.minute, m_time.time.second);

        return String(tempString);
    }

    String ToUnformattedString(void)
    {
        char tempString[static_cast<int>(Constant::TimeStringLength) + 1] = {0};

        snprintf(tempString, static_cast<int>(Constant::TimeStringLength),
                    "%02d%02d%02d%02d%02d%02d",
                    m_time.time.year, m_time.time.month, m_time.time.day,
                    m_time.time.hour, m_time.time.minute, m_time.time.second);

        return String(tempString);
    }
};
