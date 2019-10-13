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
#include <base/address.h>
#include <base/ip.h>
#include <base/ethernet.h>
#include <service/logmanager.h>

#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <linux/if_link.h>

typedef std::multimap<String, Address*> IpMap;
typedef std::pair<String, Address*> IpMapPair;

/**
 * @class EthHandler
 * @author Chirag
 * @date 25/11/2018
 * @file ethhandler.h
 * @brief Layer that handles ethernet packets and passes on to protocol handler
 */
class EthHandler
{
protected:

    // Common lock object
    Mutex m_commonLock;
    // Logging object
    Log m_log;
    // Socket descriptor
    int m_sockFd;
    // device name
    String m_device;
    // Address object for device
    Address* m_address;
    // Port to use
    u16_t m_port;
    // L4 type for posix
    int m_l4Type;
    // Protocol type for posix
    int m_protocol;
    // Transmission flags
    fd_set m_txFlags;
    // Reception flags
    fd_set m_rxFlags;

    EthHandler(const EthHandler& rhs) = delete;
    EthHandler& operator=(const EthHandler& rhs) = delete;
    EthHandler() = delete;

    EthHandler(String name,
                String device,
                Address* address,
                u16_t port,
                int l4Type,
                int protocol);
    virtual ~EthHandler();
    static void AddressToSockAddr(Address* address, u16_t port,
                                struct sockaddr_in* sockAddr);
    static void SockAddrToIp(struct sockaddr_in* sockAddr, Ip& ip, u16_t& port);

    int Send(u8_t* packet, int length, struct sockaddr_in& remote);
    int Receive(u8_t* packet, int maxLength, struct sockaddr_in* remote);

public:

    static int EnumerateInterfaces(IpMap& interfaceMap, Log& log);
    static bool GetPublicIp(Ip& retIp);

    String GetDeviceName(void) { return m_device; }
    Address* GetLocalAddress(void) { return m_address; }
    u16_t GetLocalPort(void) { return m_port; }
    int GetL4Type(void) { return m_l4Type; }
    int GetProtocol(void) { return m_protocol; }

    bool IsPacketSent(u64_t waitUs);
    bool IsPacketReceived(u64_t waitUs);
};

/**
 * @class IpMulticast
 * @author Chirag
 * @date 14/12/2018
 * @file ethhandler.h
 * @brief IPv4 multicast sender via UDP socket
 */
class IpMulticast : public EthHandler
{
private:

    // Multicast group address
    Ip* m_mcGroup;
    // Multicast group port
    u16_t m_mcPort;
    // Converted multicast group address
    struct sockaddr_in m_mcGroupSockAddr;

    IpMulticast(const IpMulticast& right) = delete;
    IpMulticast& operator = (const IpMulticast& right) = delete;
    IpMulticast() = delete;

public:

    IpMulticast(String device, const Ip& address, u16_t port, const Ip& mcGroup, u16_t mcPort);
    ~IpMulticast();

    Ip* GetLocalIp(void) { return dynamic_cast<Ip*>(m_address); }
    Ip* GetMcIp(void) { return m_mcGroup; }
    u16_t GetMcPort(void) { return m_mcPort; }

    int Send(u8_t* packet, int length);
    int ReceiveMc(u8_t* packet, int maxLength, Ip& remote, u16_t& remotePort);
};

/**
 * @class Udp
 * @author Chirag
 * @date 08/01/2019
 * @file ethhandler.h
 * @brief UDP type communication class
 */
class Udp : public EthHandler
{
private:

    // UDP target IP object
    Ip* m_target;
    // UDP target port
    u16_t m_targetPort;

public:

    Udp(String device, const Ip& address, u16_t port);
    ~Udp();

    int SendTo(u8_t* packet, int length);
};
