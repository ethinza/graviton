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
#include <service/ethhandler.h>
#include <utility/elapsedtime.h>
#include <protocol/protocol.h>

#include <list>

typedef std::list<struct Protocol::Interface> IntfList;

/**
 * @class Discovery
 * @author Chirag
 * @date 16/12/2018
 * @file discovery.h
 * @brief StorageZ discovery protocol
 */
class DiscoveryThread : public Thread
{
private:

    // All available IP interface list
    IpMap m_ipIntfMap;
    // Multicast list handlers
    IpMulticast* m_mc;
    // Timer to track retries/heartbeats etc
    ElapsedTime m_elapsedTime;
    // 1s counter
    u32_t m_secondCounter;
    // Actively used interface list for multiple purposes
    IntfList m_usableIntfList;
    // Scratch-pad used to create packets
    Packet m_packet;
    // Total storage size
    u64_t m_storageSize;
    // Bitmask for all features supported
    u32_t m_featureBits;
    // Public IP
    Ip m_publicIp;

    DiscoveryThread(const DiscoveryThread& rhs) = delete;
    DiscoveryThread& operator=(const DiscoveryThread& rhs) = delete;
    DiscoveryThread();

    void InitializeStorage(void);
    void InitializeMcGroups(void);
    void ClearMcGroups(void);

    void PrepareRegistrationPacket(void);
    void SendHeartbeatToRemoteOffice(void);
    void SendHeartbeatToMcGroup(void);

    void ProcessSecondTick(void);
    void ProcessTenSecondTick(void);
    void ProcessOneMinuteTick(void);

public:

    /**
     * @brief Creates singleton object
     * @return 
     */
    static DiscoveryThread& GetInstance(void)
    {
        static DiscoveryThread sDiscovery;
        return sDiscovery;
    }

    void AddFeature(Protocol::Feature feature)
    {
        m_featureBits |= static_cast<u32_t>(feature);
    }
    u32_t GetFeatureBits(void) { return m_featureBits; }

    ~DiscoveryThread();
    void Initialize(void) override;
    void Run(void) override;
    void Pause(void) override;
    void Resume(void) override;
    void Stop(void) override;

};

#define Discovery DiscoveryThread::GetInstance()
