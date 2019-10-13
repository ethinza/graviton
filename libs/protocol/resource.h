///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Copyright (c) 2019 Ethinza Inc., All rights reserved                      //
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

/**
 * @class Configuration
 * @author Chirag
 * @date 13/01/2019
 * @file resource.h
 * @brief Configuration thread which holds information about nodes
 */
class ConfigurationThread : public Thread
{
private:

    ConfigurationThread(const ConfigurationThread& rhs) = delete;
    ConfigurationThread& operator=(const ConfigurationThread& rhs) = delete;

public:

    /**
     * @brief Creates singleton object
     * @return 
     */
    static ConfigurationThread& GetInstance(void)
    {
        static ConfigurationThread sConfiguration;
        return sConfiguration;
    }

    ConfigurationThread();
    ~ConfigurationThread();

    void Initialize(void) override;
    void Run(void) override;
    void Pause(void) override;
    void Resume(void) override;
    void Stop(void) override;

    void AddCommand(String section, String key, String value);
};

#define Configuration ConfigurationThread::GetInstance()
