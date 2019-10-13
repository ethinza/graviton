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
#pragma once

#include <ethinza.h>
#include <utility/stringutility.h>

#include <map>

// Levels assigned for log statement
enum class LogLevel
{
    Debug = 0,
    Verbose = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Off = 5,

    LastLogLevel
};

// Controls where a print statement can go
enum class LogTarget
{
    None = 0,
    File = (1 << 1),
    Shell = (1 << 2),
    Console = (1 << 3),

    LastLogTarget
};

// forward declaration
class Log;

// Log object map type
typedef std::map<String, Log*> LogMap;

/**
 * @class LogManager
 * @author Chirag
 * @date 09/12/2018
 * @file logmanager.h
 * @brief Logging service manager
 */
class LogManager
{
private:

    // Map containing log objects
    LogMap m_logMap;
    // Lock logging object
    Mutex m_mutex;
    // Targets chosen
    u32_t m_logTarget;
    // Log file pointer
    FILE* m_fp;

    LogManager(const LogManager& rhs) = delete;
    LogManager& operator=(const LogManager& rhs) = delete;
    LogManager();
    ~LogManager();

    const char* LogLevelToString(LogLevel level);
    const char* GetFileName(void);

public:

    // Gets log manager instance
    static LogManager& GetInstance(void)
    {
        static LogManager sManager;
        return sManager;
    }

    void Register(Log* log);
    void Unregister(Log* log);
    
    void AddTarget(LogTarget target) { m_logTarget |= static_cast<u32_t>(target); }
    void ClearTarget(LogTarget target) { m_logTarget &= ~(static_cast<u32_t>(target)); }

    void Print(Log* log, LogLevel printLevel, String logText);
};

#define LogM LogManager::GetInstance()

/**
 * @class Log
 * @author Chirag
 * @date 09/12/2018
 * @file logmanager.h
 * @brief Caller logging object
 */
class Log
{
private:

    // Component name
    String m_component;
    // Default log level
    LogLevel m_defaultLevel;

public:

    // ctor
    Log(String component)
    :   m_component(component),
        m_defaultLevel(LogLevel::Info)
    {
        LogM.Register(this);
    }

    // dtor
    ~Log()
    {
        LogM.Unregister(this);
    }
    
    String& GetComponent(void) { return m_component; }
    LogLevel GetDefaultLevel(void) { return m_defaultLevel; }
    void SetDefaultLevel(LogLevel newLevel) { m_defaultLevel = newLevel; }

    void Debug(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        LogM.Print(this, LogLevel::Debug, StringUtility::Format(format, args));
        va_end(args);
    }

    void Verbose(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        LogM.Print(this, LogLevel::Verbose, StringUtility::Format(format, args));
        va_end(args);
    }

    void Info(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        LogM.Print(this, LogLevel::Info, StringUtility::Format(format, args));
        va_end(args);
    }

    void Warning(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        LogM.Print(this, LogLevel::Warning, StringUtility::Format(format, args));
        va_end(args);
    }

    void Error(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        LogM.Print(this, LogLevel::Error, StringUtility::Format(format, args));
        va_end(args);
    }
};

/**
 * @class EthCommonLog
 * @author Chirag
 * @date 09/12/2018
 * @file logmanager.h
 * @brief Common logging object
 */
class EthCommonLog
{
private:

    // Log object
    Log m_log;

    EthCommonLog(const EthCommonLog& rhs) = delete;
    EthCommonLog& operator = (const EthCommonLog& rhs) = delete;
    // default ctor
    EthCommonLog()
    :   m_log("Common")
    {}

    // dtor
    ~EthCommonLog() {}

public:

    /**
     * @brief Singleton for CommonLog
     * @return singleton instance
     */
    static Log& GetInstance(void)
    {
        static EthCommonLog sCommonLog;
        return sCommonLog.m_log;
    }
};

#define CommonLog EthCommonLog::GetInstance()
