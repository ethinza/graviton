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

#include "resource.h"
#include <utility/stringutility.h>

// default ctor
ConfigurationThread::ConfigurationThread()
:   Thread("Configuration")
{
}

// dtor
ConfigurationThread::~ConfigurationThread()
{
}

// Static method to collect ini file content
void AddConfigValue(String section, String key, String value)
{
    Configuration.AddCommand(section, key, value);
}

void ConfigurationThread::AddCommand(String section, String key, String value)
{
    m_log.Info("Adding [%s] %s=%s", section.c_str(), key.c_str(), value.c_str());
}

void ConfigurationThread::Initialize(void)
{
    StringUtility::InterpretIniFile(String("nodemap.conf"), &AddConfigValue);
}

void ConfigurationThread::Run(void)
{
}

void ConfigurationThread::Pause(void)
{
}

void ConfigurationThread::Resume(void)
{
}

void ConfigurationThread::Stop(void)
{
}
