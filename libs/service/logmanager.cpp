///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2018-2019 Ethinza Inc, All rights reserved
//
// This file contains TRADE SECRETs of ETHINZA INC and is CONFIDENTIAL.
// Without written consent from ETHINZA INC, it is prohibited to disclose or
// reproduce its contents, or to manufacture, use or sell it in whole or part.
// Any reproduction of this file without written consent of ETHINZA INC is a 
// violation of the copyright laws and is subject to civil liability and 
// criminal prosecution.
//
///////////////////////////////////////////////////////////////////////////////
#include "logmanager.h"

#include <utility/stringbuilder.h>
#include <utility/fileutility.h>
#include <utility/osinterface.h>

#include <stdarg.h>

#define LOG_FOLDER "ethinza"

// default ctor
LogManager::LogManager()
{
    m_logTarget = static_cast<u32_t>(LogTarget::File);

    FileUtility::CreateIfDoesNotExist(LOG_FOLDER);

    m_fp = fopen(GetFileName(), "w");
    if (m_fp != nullptr) {
        fprintf(m_fp, "///////////////////////////////////////////////////////////////////////////////\n");
        fprintf(m_fp, "Copyright (c) 2019 Ethinza Inc, All rights reserved\n");
    }
}

// dtor
LogManager::~LogManager()
{
    if (m_fp != nullptr) {
        fprintf(m_fp, "///////////////////////////////////////////////////////////////////////////////\n");
        fclose(m_fp);
    }
}

const char* LogManager::GetFileName(void)
{
    StringBuilder sb;
    sb.Append("%s/%s.log", LOG_FOLDER, Osi.CurrentTime().ToUnformattedString().c_str());
    return sb.GetText();
}

// Registers log object
void LogManager::Register(Log* log)
{
    LockGuard guard(m_mutex);
    
    m_logMap[log->GetComponent()] = log;
}

// Unregisters log object
void LogManager::Unregister(Log* log)
{
    LockGuard guard(m_mutex);
    
    m_logMap.erase(log->GetComponent());
}

// Returns string associated with level
const char* LogManager::LogLevelToString(LogLevel level)
{
    static const char* sLevelStrings[static_cast<int>(LogLevel::LastLogLevel)] = 
            { "D", "V", "I", "W", "E", "O" };
    return sLevelStrings[static_cast<int>(level)];
}

/**
 * @brief Submits log statement for printing
 * @param log
 * @param printLevel
 * @param logText
 */
void LogManager::Print(Log* log, LogLevel printLevel, String logText)
{
    StringBuilder sb;

    // Currently, print to console and work on various aspects of logging
    sb.Append("%s", Osi.CurrentTime().ToString().c_str());
    sb.Append(" # %s %8.8s # ", LogLevelToString(printLevel), log->GetComponent().c_str());
    sb.AppendLine("%s", logText.c_str());

    if ((m_logTarget & static_cast<u32_t>(LogTarget::File)) && m_fp)
        fprintf(m_fp, "%s", sb.GetText());

    if (m_logTarget & static_cast<u32_t>(LogTarget::Shell))
        printf("%s\n", logText.c_str());
}
