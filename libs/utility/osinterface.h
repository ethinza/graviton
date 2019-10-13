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
#include <base/ethtime.h>

#include <unistd.h>
#include <sys/types.h>
#include <list>

// Operating System type
enum class OsType
{
    Unknown = 0,
    Centos = 1,
    Debian = 2,
    Fedora = 3,
    Rhel = 4,
    Windows = 5,
    Android = 6,
    Ios = 7,

    LastOsType
};

// Type of processor
enum class ProcessorType
{
    Unknown = 0,
    Intel64 = 1,
    Intel32 = 2,
    Amd32 = 3,
    Amd64 = 4,
    IbmPower8 = 5,
    
    LastProcessorType
};

/**
 * @class OsInterface
 * @author Chirag
 * @date 18/11/2018
 * @file osinterface.h
 * @brief OS interface utility
 */
class OsInterface
{
private:

    // Flag to suggest if this machine is big endian
    bool m_bigEndian;
    uid_t m_uid;
    OsType m_osType;
    ProcessorType m_processorType;
    int m_totalProcessors;
    u64_t m_ticksPerSec;
    u64_t m_ramSize;
    u64_t m_hddSize;
    u64_t m_hddFreeSize;
    String m_osName;
    int m_osVersion;
    String m_cpuModel;
    bool m_isVirtualizationSupported;

    OsInterface();
    ~OsInterface() {}
    OsInterface(const OsInterface& rhs) = delete;
    OsInterface& operator=(const OsInterface& rhs) = delete;

    uid_t FindUser(void);

public:

    // Gets singleton object
    static OsInterface& GetInstance(void)
    {
        static OsInterface intf;
        return intf;
    }

    /**
     * @brief Sleeps calling thread for specified time
     * @param microseconds
     */
    void SleepUs(u32_t microseconds) { usleep(microseconds); }
    void SleepMs(u32_t milliseconds) { usleep(milliseconds * 1000); }

    bool IsStdinEvent(u32_t waitMicros);

    u64_t CurrentTicks(void);
    EthTime CurrentTime(void);
    EthTime CurrentUtcTime(void);

    int GetDriveList(StringList& driveList);
    int Execute(const String& command, String& output);
    bool IsExecutable(const String& executable);
    
    bool UpdateEtcHosts(StringList& ipList, StringList& hostList, StringList& fqdnList);
    
    void SetSystemDetails(void);
    bool IsBigEndianMachine(void) { return m_bigEndian; }
    OsType GetOsType(void) { return m_osType; }
    ProcessorType GetProcessorType(void) { return m_processorType; }
    int GetTotalProcessors(void) { return m_totalProcessors; }
    u64_t GetClockTicksPerSecond(void) { return m_ticksPerSec; }
    u64_t GetRamSize(void) { return m_ramSize; }
    u64_t GetHddSize(void) { return m_hddSize; }
    u64_t GetHddFreeSize(void) { return m_hddFreeSize; }
    String GetOsName(void) { return m_osName; }
    int GetOsVersion(void) { return m_osVersion; }
    String GetCpuModel(void) { return m_cpuModel; }
    bool IsVirtualizationSupported(void) { return m_isVirtualizationSupported; }
};

#define Osi OsInterface::GetInstance()
