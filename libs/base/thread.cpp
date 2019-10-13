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

#include "thread.h"
#include <service/threadmonitor.h>
#include <utility/osinterface.h>

/**
 * @brief user ctor
 * @param name
 * @return 
 */
Thread::Thread(String name)
:   m_name(name),
    m_state(ThreadState::Created),
    m_thread(nullptr),
    m_handle(ThreadMon.RegisterThread(this)),
    m_log(name)
{
    m_log.Debug("%s created", m_name.c_str());
}

/**
 * @brief dtor
 * @return 
 */
Thread::~Thread()
{
    Stop();

    delete m_thread;
    m_state = ThreadState::Destroyed;
    ThreadMon.UnregisterThread(m_handle);
    
    m_log.Debug("Thread destroyed");
}

/**
 * @brief Initialized thread
 */
void Thread::Initialize(void)
{
    m_state = ThreadState::Initialized;
    m_log.Debug("Thread initialized");
}

/**
 * @brief Main execution function
 */
void Thread::Run(void)
{
    m_state = ThreadState::Running;
    m_log.Debug("Thread running");
}

void Thread::RunBegin(void)
{
    m_state = ThreadState::Running;
}

void Thread::RunEnd(void)
{
    m_state = ThreadState::Stopped;
}

/**
 * @brief Pauses thread
 */
void Thread::Pause(void)
{
    m_state = ThreadState::Paused;
    m_log.Debug("Thread paused");
}

/**
 * @brief Resumes paused thread
 */
void Thread::Resume(void)
{
    m_state = ThreadState::Running;
    m_log.Debug("Thread resumed");
}

/**
 * @brief Stops thread
 */
void Thread::Stop(void)
{
    m_log.Debug("About to stop thread if found running");

    if ((m_state != ThreadState::Stopped)
        && (m_state != ThreadState::Stopping))
    {
        m_state = ThreadState::Stopping;

        if (m_thread)
        {
            if (m_thread->joinable()) m_thread->join();
            while (m_state != ThreadState::Stopped) Osi.SleepMs(10);
        }

        m_state = ThreadState::Stopped;

        m_log.Debug("Thread stopped");
    }
}

/**
 * @brief Starts executing thread in normal running state
 */
void Thread::Start(void)
{
    m_thread = new std::thread( [this] { this->Run(); } );
    m_thread->detach();

    m_log.Debug("Thread started");
}
