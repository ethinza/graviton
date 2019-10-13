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
#include "message.h"
#include <user/userinterface.h>
#include <service/logmanager.h>

#include <string>
#include <queue>
#include <mutex>
#include <thread>

// States assigned to a thread
enum class ThreadState
{
    Unknown = 0,
    Created = 1,
    Initialized = 2,
    Running = 3,
    Paused = 4,
    Stopping = 5,
    Stopped = 6,
    Destroyed = 7,

    LastThreadState
};

/**
 * @class Thread
 * @author Chirag
 * @date 11/11/2018
 * @file thread.h
 * @brief Thread utility class
 */
class Thread
{
private:

    Thread() = delete;
    Thread(const Thread& rhs) = delete;
    Thread& operator=(const Thread& rhs) = delete;

protected:

    // Name for thread
    String m_name;
    // Current state of thread
    ThreadState m_state;
    // Thread object
    std::thread* m_thread;
    // Handle returned by ThreadMonitor
    int m_handle;
    // Logging object
    Log m_log;

    void RunBegin(void);
    void RunEnd(void);

public:

    Thread(String name);
    virtual ~Thread();
    void Start(void);
    
    String GetName(void) { return m_name; }
    ThreadState GetState(void) { return m_state; }

    virtual void Initialize(void);
    virtual void Run(void);
    virtual void Pause(void);
    virtual void Resume(void);
    virtual void Stop(void);
};
