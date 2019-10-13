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

#include <chrono>

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds TimeMs;
typedef std::chrono::microseconds TimeUs;
typedef std::chrono::nanoseconds TimeNs;
typedef std::chrono::duration<float> TimeDurationFloat;

#define TIME_1S_NS 1000000000LL

/**
 * @class ElapsedTime
 * @author Chirag
 * @date 24/11/2018
 * @file elapsedtime.h
 * @brief Wrapper to find elapsed time between two set points
 */
class ElapsedTime
{
private:

    Time::time_point m_start;

    ElapsedTime(const ElapsedTime& rhs) = delete;
    ElapsedTime& operator=(const ElapsedTime& rhs) = delete;

public:

    /**
     * @brief default ctor
     * @return 
     */
    ElapsedTime()
    {
        m_start = Time::now();
    }

    /**
     * @brief dtor
     * @return 
     */
    ~ElapsedTime()
    {
    }

    /**
     * @brief Marks starting point
     */
    void Mark(void)
    {
        m_start = Time::now();
    }

    /**
     * @brief Gets duration between Mark point and this function call
     * @return nanoseconds
     */
    u64_t DurationNs(void)
    {
        Time::time_point current = Time::now();
        TimeDurationFloat duration = current - m_start;
        return (std::chrono::duration_cast<TimeNs>(duration)).count();
    }
};
