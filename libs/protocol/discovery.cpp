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

#include "discovery.h"
#include <utility/osinterface.h>
#include <utility/memoryutility.h>

/**
 * @brief ctor
 * @return 
 */
DiscoveryThread::DiscoveryThread()
:   Thread("Discovery"),
    m_secondCounter(0),
    m_featureBits(0)
{
    m_log.Warning("Running on %s machine", Osi.IsBigEndianMachine() ? "BIG ENDIAN" : "LITTLE ENDIAN");

    m_mc = nullptr;
}

/**
 * @brief dtor
 * @return 
 */
DiscoveryThread::~DiscoveryThread()
{
    
}

void DiscoveryThread::Initialize(void)
{
    m_usableIntfList.clear();
    InitializeStorage();
    InitializeMcGroups();

    Thread::Initialize();
}

/**
 * @brief Initializes storage drives available on this node and their relevant information
 */
void DiscoveryThread::InitializeStorage(void)
{
    StringList driveList;
    Osi.GetDriveList(driveList);

    m_storageSize = 0;
    for (String drive : driveList)
    {
        m_log.Debug("Found drive %s", drive.c_str());
        m_storageSize++; // todo fix
    }
    
    // todo Get total and free size
}

/**
 * @brief Initializes all interfaces and joins multicast groups from relevant ones
 */
void DiscoveryThread::InitializeMcGroups(void)
{
    // todo set unique, persistent node id thru multicast group
    ProtoInst.SetNodeId(1);

    Ip mcIp(MULTICAST_GROUP_STR);

    m_log.Debug("Initializing multicast groups");

    EthHandler::EnumerateInterfaces(m_ipIntfMap, m_log);
    // Check ini file and open a multicast type group
    for (IpMap::iterator it = m_ipIntfMap.begin(); it != m_ipIntfMap.end(); ++it)
    {
        if (it->second->GetType() == AddressType::Ipv4)
        {
            Ip* ip = reinterpret_cast<Ip*>(it->second);
            m_log.Info("Address %s u32=%08x %s type=%d active=%d",
                it->first.c_str(), ip->ToU32(), ip->ToString().c_str(), ip->GetIpAddressType(),
                ip->IsActive());

            if (((ip->GetIpAddressType() == IpAddressType::ClassA)
                    || (ip->GetIpAddressType() == IpAddressType::ClassB)
                    || (ip->GetIpAddressType() == IpAddressType::ClassC))
                && ip->IsActive())
            {
                m_log.Warning("Using interface %s with %s to join multicast group %s",
                                it->first.c_str(), ip->ToString().c_str(),
                                mcIp.ToString().c_str());

                // Add to usable interface list
                struct Protocol::Interface intf;
                ip->ToArray(intf.address);
                // todo capabilities based on configuration
                intf.cap_bits = static_cast<u32_t>(Protocol::InterfaceCapability::Lan)
                            | static_cast<u32_t>(Protocol::InterfaceCapability::Wan)
                            | static_cast<u32_t>(Protocol::InterfaceCapability::Isl)
                            | static_cast<u32_t>(Protocol::InterfaceCapability::Ethernet1g);
                intf.type = static_cast<u8_t>(Protocol::InterfaceType::Ipv4);
                m_usableIntfList.push_back(intf);

                m_mc = new IpMulticast(it->first,
                    *ip, static_cast<u16_t>(Protocol::Constant::Port),
                    mcIp, static_cast<u16_t>(Protocol::Constant::Port));

                // todo multiple interfaces in the mc group
                break;
            }
        }
    }

    // from shell, check "netstat -ng"

}

/**
 * @brief Clears all mc groups created
 */
void DiscoveryThread::ClearMcGroups(void)
{
    m_log.Debug("Clearing multicast groups");

    delete m_mc;
    m_mc = nullptr;

    for (IpMap::iterator it = m_ipIntfMap.begin(); it != m_ipIntfMap.end(); ++it)
        delete it->second;
}

void DiscoveryThread::Run(void)
{
    Thread::RunBegin();

    if (m_mc == nullptr) m_log.Error("Multicast could not be established");
    else m_log.Warning("Starting to use multicast group");

    m_elapsedTime.Mark();

    while (m_state != ThreadState::Stopping)
    {
        Osi.SleepMs(100);
        
        if (m_elapsedTime.DurationNs() >= TIME_1S_NS)
        {
            ProcessSecondTick();
            m_elapsedTime.Mark();
        }
    }

    m_log.Debug("Main loop stopped");
    Thread::RunEnd();
}

void DiscoveryThread::Pause(void)
{
    Thread::Pause();
}

void DiscoveryThread::Resume(void)
{
    Thread::Resume();
}

void DiscoveryThread::Stop(void)
{
    Thread::Stop();

    ClearMcGroups();
}

/**
 * @brief Processes 1s tick
 */
void DiscoveryThread::ProcessSecondTick(void)
{
    if ((m_secondCounter % 10) == 0) ProcessTenSecondTick();
    if ((m_secondCounter % 60) == 0) ProcessOneMinuteTick();
    ++m_secondCounter;
}

/**
 * @brief Processes 10s tick
 */
void DiscoveryThread::ProcessTenSecondTick(void)
{
    m_log.Debug("10s tick");

    SendHeartbeatToMcGroup();
}

/**
 * @brief Processes 1minute tick
 */
void DiscoveryThread::ProcessOneMinuteTick(void)
{
    m_log.Debug("1m tick");

    EthHandler::GetPublicIp(m_publicIp);
    SendHeartbeatToRemoteOffice();
}

// Prepares registration packet to send
void DiscoveryThread::PrepareRegistrationPacket(void)
{
    u8_t totalInterfaces = m_usableIntfList.size();
    u16_t packetLength = sizeof(struct Protocol::Registration)
                        + ((totalInterfaces - 1) * sizeof(struct Protocol::Interface));
    // todo remember sequence and retry count and processing per packet type
    u16_t sequence = ProtoInst.GetNextSequence();
    struct Protocol::Registration* reqPacket = &(m_packet.registration);
    struct Protocol::Interface* intfs = reqPacket->interfaces;

    memset(reqPacket, 0, packetLength);
    ProtoInst.PrepareHeader(reqPacket->header,
                            Protocol::PacketId::Registration,
                            Protocol::PacketType::Notification,
                            sequence,
                            0,
                            packetLength);
    reqPacket->app_type = MemoryUtility::CheckAndSwap(static_cast<u8_t>(ProtoInst.GetAppType()));
    reqPacket->app_version = MemoryUtility::CheckAndSwap(ProtoInst.GetAppVersion());
    reqPacket->storage_size = MemoryUtility::CheckAndSwap(m_storageSize);
    reqPacket->feature_bits = MemoryUtility::CheckAndSwap(m_featureBits);
    m_publicIp.ToArray(reqPacket->public_ip);

    reqPacket->total_interfaces = MemoryUtility::CheckAndSwap(totalInterfaces);
    int idx = 0;
    for (struct Protocol::Interface intf : m_usableIntfList)
    {
        memcpy(intfs[idx].address, intf.address, sizeof(intf.address));
        intfs[idx].type = MemoryUtility::CheckAndSwap(intf.type);
        intfs[idx].cap_bits = MemoryUtility::CheckAndSwap(intf.cap_bits);
        ++idx;
    }
}

/**
 * @brief Prepares and sends heartbeat to multicast group
 */
void DiscoveryThread::SendHeartbeatToMcGroup(void)
{
    if (m_usableIntfList.empty())
    {
        m_log.Warning("No usable interface found to send heartbeat to multicast group");
        return;
    }

    PrepareRegistrationPacket();
    m_mc->Send(m_packet.scratchPad, MemoryUtility::CheckAndSwap(m_packet.registration.header.length));
    m_log.Debug("Sent registration packet to multicast group");
}

/**
 * @brief Prepares and sends heartbeat to remote offices
 */
void DiscoveryThread::SendHeartbeatToRemoteOffice(void)
{
    if (m_usableIntfList.empty())
    {
        m_log.Warning("No usable interface found to send heartbeat to remote office");
        return;
    }

    PrepareRegistrationPacket();
}
