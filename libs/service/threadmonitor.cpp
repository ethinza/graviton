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

#include "threadmonitor.h"
#include <service/logmanager.h>
#include <protocol/discovery.h>
#include <protocol/resource.h>

/**
 * @brief Singleton access method
 * @return singleton object
 */
ThreadMonitor& ThreadMonitor::GetInstance(void)
{
    static ThreadMonitor singleton;
    return singleton;
}

/**
 * @brief default ctor
 * @return 
 */
ThreadMonitor::ThreadMonitor()
:   m_handleCounter(0)
{
}

/**
 * @brief dtor
 * @return 
 */
ThreadMonitor::~ThreadMonitor()
{
}

/**
 * @brief Registers a thread object to monitoring
 * @param thread
 */
int ThreadMonitor::RegisterThread(Thread* thread)
{
    LockGuard lock(m_mutex);

    int returnHandle = ++m_handleCounter; 
    m_threadMap[returnHandle] = thread;

    CommonLog.Debug("Thread %s registered at %d", thread->GetName().c_str(), returnHandle);
    return returnHandle;
}

/**
 * @brief Unregisters a thread object from monitoring
 * @param handle
 */
void ThreadMonitor::UnregisterThread(int handle)
{
    LockGuard lock(m_mutex);

    CommonLog.Debug("Thread at %d unregistered", handle);
    m_threadMap.erase(handle);
}

/**
 * @brief Initializes all registered threads
 */
void ThreadMonitor::InitializeThreads(void)
{
    Discovery.Initialize();
    Configuration.Initialize();
}

/**
 * @brief Starts all registered threads
 */
void ThreadMonitor::StartThreads(void)
{
    LockGuard lock(m_mutex);

    for (ThreadMap::iterator it = m_threadMap.begin(); it != m_threadMap.end(); ++it)
    {
        it->second->Start();
    }
}

/**
 * @brief Pauses all registered threads
 */
void ThreadMonitor::PauseThreads(void)
{
    LockGuard lock(m_mutex);

    for (ThreadMap::iterator it = m_threadMap.begin(); it != m_threadMap.end(); ++it)
    {
        it->second->Pause();
    }
}

/**
 * @brief Resumes all registered threads
 */
void ThreadMonitor::ResumeThreads(void)
{
    LockGuard lock(m_mutex);

    for (ThreadMap::iterator it = m_threadMap.begin(); it != m_threadMap.end(); ++it)
    {
        it->second->Resume();
    }
}

/**
 * @brief Stops all registered threads
 */
void ThreadMonitor::StopThreads(void)
{
    LockGuard lock(m_mutex);

    for (ThreadMap::iterator it = m_threadMap.begin(); it != m_threadMap.end(); ++it)
    {
        it->second->Stop();
    }
}
