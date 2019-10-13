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
#include <base/thread.h>

#include <map>

typedef std::map<int, Thread*> ThreadMap;

/**
 * @class ThreadMonitor
 * @author Chirag
 * @date 17/11/2018
 * @file threadmonitor.h
 * @brief ThreadMonitor is singleton class which keeps track of all threads
 *      created by using Thread base class
 */
class ThreadMonitor
{
private:

    ThreadMap m_threadMap;
    Mutex m_mutex;
    int m_handleCounter;

    ThreadMonitor();
    ~ThreadMonitor();
    ThreadMonitor(const ThreadMonitor& rhs) = delete;
    ThreadMonitor& operator=(const ThreadMonitor& rhs) = delete;

public:

    // Singleton access method
    static ThreadMonitor& GetInstance(void);

    int RegisterThread(Thread* thread);
    void UnregisterThread(int handle);

    void InitializeThreads(void);
    void StartThreads(void);
    void PauseThreads(void);
    void ResumeThreads(void);
    void StopThreads(void);
};

#define ThreadMon ThreadMonitor::GetInstance()
