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

#include "ethhandler.h"
#include <base/ip.h>
#include <base/ipv6.h>
#include <base/mac.h>
#include <utility/memoryutility.h>

#include <unistd.h>
#include <fcntl.h>
#include <netpacket/packet.h>

/**
 * @brief ctor
 * @param name object name/label
 * @param device interface name
 * @param address address to use for interface
 * @param port associated port
 * @param l4Type posix l4 type SOCK
 * @param protocol posix protocol type IPPROTO
 * @return 
 */
EthHandler::EthHandler(String name,
                        String device,
                        Address* address,
                        u16_t port,
                        int l4Type,
                        int protocol)
:   m_log(name),
    m_sockFd(-1),
    m_device(device),
    m_address(address),
    m_port(port),
    m_l4Type(l4Type),
    m_protocol(protocol)
{
    m_log.Info("[%s] Trying to open %s:%d L4=%d proto=%d",
            device.c_str(), address->ToString().c_str(), port, l4Type, protocol);

    // open a socket
    //if (l4Type == SOCK_DGRAM)
        //m_sockFd = socket(PF_INET, SOCK_RAW, protocol);
    //else
        //sockfd = socket(AF_INET, l4Type, protocol);
    m_sockFd = socket(PF_INET, l4Type, protocol);

    if (m_sockFd == -1) {
        m_log.Error("[%s:%d] IP Socket open fail with %d(%s)",
                device.c_str(), m_sockFd, errno, strerror(errno));
    } else {
        m_log.Debug("[%s:%d] socket opened", device.c_str(), m_sockFd);
    }

    // make it non-blocking
    {
        int flags = fcntl(m_sockFd, F_GETFL, 0);
        if (fcntl(m_sockFd, F_SETFL, flags | O_NONBLOCK) < 0) {
            m_log.Warning("[%s:%d] Error %d(%s) setting non-blocking socket",
                            device.c_str(), m_sockFd, errno, strerror(errno));
        } else {
            m_log.Debug("[%s:%d] socket set to non-blocking", device.c_str(), m_sockFd);
        }
    }

    // Enable address reuse
    {
        int on = 1;
        //int off = 0;

        setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        setsockopt(m_sockFd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
        //setsockopt(m_sockFd, IPPROTO_IP, IP_PKTINFO, &on, sizeof(on));
        //setsockopt(m_sockFd, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof(on));
        //setsockopt(m_sockFd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));
        m_log.Info("[%s:%d] set reuse-address option on socket successfully",
                    device.c_str(), m_sockFd);
    }

    // bind address
    {
        struct sockaddr_in localAddr;
        AddressToSockAddr(address, port, &localAddr);

        if (bind(m_sockFd, reinterpret_cast<struct sockaddr*>(&localAddr), sizeof(localAddr)) < 0) {
            m_log.Error("[%s:%d] IP seocket bind error %d(%s)",
                device.c_str(), m_sockFd, errno, strerror(errno));

            close(m_sockFd);
            m_sockFd = -1;
        } else {
            m_log.Error("[%s:%d] bind address %s:%d successfully",
                            device.c_str(), m_sockFd, address->ToString().c_str(), port);
        }
    }
}

/**
 * @brief dtor
 * @return 
 */
EthHandler::~EthHandler()
{
    if (m_sockFd != -1) {
        m_log.Info("[%s:%d] removing socket", m_device.c_str(), m_sockFd);
        close(m_sockFd);
    }
}

/**
 * @brief Converts address based on type to posic address
 * @param address internal address object
 * @param port port addressed
 * @param sockAddr returned converted address
 */
void EthHandler::AddressToSockAddr(Address* address, u16_t port,
                                    struct sockaddr_in* sockAddr)
{
    memset(sockAddr, 0, sizeof(struct sockaddr_in));

    //sockAddr.sin6_family = MemoryUtility::Swap(AF_INET6);
    //sockAddr.sin6_port = MemoryUtility::Swap(port);

    if (address->GetType() == AddressType::Ipv4) {
        Ip* ip = dynamic_cast<Ip*>(address);

        sockAddr->sin_family = AF_INET;
        sockAddr->sin_addr.s_addr = ip->ToU32();
        sockAddr->sin_port = MemoryUtility::Swap(port);
    }
}

/**
 * @brief Converts socket address to IPv4 address and port
 * @param sockAddr
 * @param ip
 * @param port
 */
void EthHandler::SockAddrToIp(struct sockaddr_in* sockAddr, Ip& ip, u16_t& port)
{
    if (sockAddr->sin_family == AF_INET)
    {
        ip = Ip(inet_ntoa(sockAddr->sin_addr));
        port = MemoryUtility::Swap(static_cast<u16_t>(sockAddr->sin_port));
    }
}

/**
 * @brief Enumerates ethernet network interfaces
 * @return total interfaces found
 */
int EthHandler::EnumerateInterfaces(IpMap& interfaceMap, Log& log)
{
    interfaceMap.clear();

    struct ifaddrs* ifaddr;
    struct ifaddrs* ifa;
    int family, s, n;
    char host[NI_MAXHOST];
    int intfCount = 0;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return intfCount;
    }

    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == NULL) continue;

        family = ifa->ifa_addr->sa_family;

        log.Debug("%-8s %s (%d)",
                      ifa->ifa_name, // adaptor name
                      (family == AF_PACKET) ? "AF_PACKET" :
                      (family == AF_INET) ? "AF_INET" :
                      (family == AF_INET6) ? "AF_INET6" : "???",
                      family);

        String name(ifa->ifa_name);

        if (family == AF_INET || family == AF_INET6) {
            s = getnameinfo(ifa->ifa_addr,
                           (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                                 sizeof(struct sockaddr_in6),
                           host,
                           NI_MAXHOST,
                           NULL,
                           0,
                           NI_NUMERICHOST);
            if (s != 0) {
                log.Error("getnameinfo() failed: %s", gai_strerror(s));
                continue;
            }

            log.Debug("\t\taddress: <%s>", host); // ipaddress

            if (family == AF_INET) {
                interfaceMap.insert(IpMapPair(name, new Ip(ifa)));
            } else {
                interfaceMap.insert(IpMapPair(name, new Ipv6(host)));
            }
        } else if (family == AF_PACKET && ifa->ifa_data != NULL) {
            struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
            char mac[64] = {0};

            for (int i = 0; i < s->sll_halen; i++){
                sprintf(&mac[i * 3], "%02x%c", (s->sll_addr[i]), (i+1 != s->sll_halen)? ':' : ' ');
            }

            log.Debug("\t\tmac: <%s>", mac);

            interfaceMap.insert(IpMapPair(name, new Mac(ifa)));

            /*
            struct rtnl_link_stats *stats = ifa->ifa_data;

            log.Debug("\t\ttx_packets = %10u; rx_packets = %10u\n"
                          "\t\ttx_bytes   = %10u; rx_bytes   = %10u",
                          stats->tx_packets, stats->rx_packets,
                          stats->tx_bytes, stats->rx_bytes);
                           */
        }
    }

    freeifaddrs(ifaddr);
    return intfCount;
}

/**
 * @brief Function to send packet
 * @param packet
 * @param length
 * @param sockaddr_in
 * @return -1 on errror or total bytes sent
 */
int EthHandler::Send(u8_t* packet, int length, struct sockaddr_in& remote)
{
    FD_CLR(m_sockFd, &m_txFlags);

    int sent = 0;
    int bytecount = 0;
    struct sockaddr* tmpaddr = reinterpret_cast<struct sockaddr*>(&remote);
    bool isTcp = (m_l4Type == SOCK_STREAM);

    while (sent < length) {
        bytecount = sendto(m_sockFd, &packet[sent], 
                            (length - sent), 0,
                            tmpaddr, sizeof(struct sockaddr_storage));
        sent += bytecount;

        if ((isTcp && (bytecount < 0))
            || (!isTcp && (sent != length))) {
            m_log.Warning("[%s:%d] packet transmission to %s:%d error %d(%s)",
                m_device.c_str(), m_sockFd,
                inet_ntoa(remote.sin_addr), remote.sin_port,
                errno, strerror(errno));

            return -1;
        }

        if (!isTcp) break;
    } // while, sent < length

    m_log.Debug("[%s:%d] packet sent to %s:%d with length %d",
        m_device.c_str(), m_sockFd,
        inet_ntoa(remote.sin_addr), remote.sin_port, sent);

    return sent;
}

/**
 * @brief Function receive packet
 * @param packet
 * @return -1 on error or total bytes received
 */
int EthHandler::Receive(u8_t* packet, int maxLength, struct sockaddr_in* remote)
{
    int bytecount;
    socklen_t addrLen = sizeof(struct sockaddr_in);

    FD_CLR(m_sockFd, &m_rxFlags);

    if ((bytecount = recvfrom(m_sockFd, packet, maxLength, 0,
                                reinterpret_cast<struct sockaddr*>(remote), &addrLen)) <= 0) {
        //if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) ???
            //return -2;

        m_log.Warning("[%s:%d] packet reception error %d(%s)",
                m_device.c_str(), m_sockFd,
                errno, strerror(errno));
        return -1;
    }

    m_log.Debug("[%s:%d] packet from %s:%d with length %d",
        m_device.c_str(), m_sockFd,
        inet_ntoa(remote->sin_addr), remote->sin_port, bytecount);

    return bytecount;
}

/**
 * @brief Check is last packet is sent on wire
 * @param waitUs time to wait in micros
 * @return true if packet is sent
 */
bool EthHandler::IsPacketSent(u64_t waitUs)
{
    struct timeval waitd;
    int ret;

    waitd.tv_sec = waitUs / 1000000;
    waitd.tv_usec = waitUs % 1000000;

    FD_ZERO(&m_txFlags);
    FD_SET(m_sockFd, &m_txFlags);

    ret = select(m_sockFd + 1, NULL, &m_txFlags, NULL, &waitd);
    if (ret < 0) {
        m_log.Warning("[%s:%d] could not read tx wait flag error %d(%s)",
                    m_device.c_str(), m_sockFd, errno, strerror(errno));
        return false;
    }

    return FD_ISSET(m_sockFd, &m_txFlags);
}

/**
 * @brief Check is any packet is available on wire
 * @param waitUs time to wait in micros
 * @return true if packet is ready to receive
 */
bool EthHandler::IsPacketReceived(u64_t waitUs)
{
    struct timeval waitd;
    int ret;

    waitd.tv_sec = waitUs / 1000000;
    waitd.tv_usec = waitUs % 1000000;

    FD_ZERO(&m_rxFlags);
    FD_SET(m_sockFd, &m_rxFlags);

    ret = select(m_sockFd + 1, &m_rxFlags, NULL, NULL, &waitd);
    if (ret < 0) {
        m_log.Warning("[%s:%d] could not read rx wait flag error %d(%s)",
                    m_device.c_str(), m_sockFd, errno, strerror(errno));
        return false;
    }

    return FD_ISSET(m_sockFd, &m_rxFlags);
}

/**
 * @brief Reads public IP address from internet provider
 * @param retIp IP object
 * @return true if successful
 */
bool EthHandler::GetPublicIp(Ip& retIp)
{
    static int RCVBUFSIZE = 1024;
    int sock;                        // Socket descriptor
    struct sockaddr_in servAddr;     // server address
    unsigned short servPort;         // server port
    char const *servIP;              // Server IP address (dotted quad)
    char const *request;             // String to send to server
    char recvBuffer[1024];     // Buffer for response string
    unsigned int requestLen;         // Length of string to send
    int bytesRcvd;                   // Bytes read in single recv()
    bool status = true;

    // Initialize server
    //servIP = "72.233.89.199";
    //servPort = 80;
    //request = "GET /automation/n09230945.asp HTTP/1.1\r\nHost: www.whatismyip.com\r\n\r\n";

    servIP = "50.16.248.221";
    servPort = 80;
    request = "GET / HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";

    // Create a reliable, stream socket using TCP
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        status = false;
    }

    if (status)
    {
        // Convert dotted decimal into binary server address.
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family      = AF_INET;
        servAddr.sin_addr.s_addr = inet_addr(servIP);
        servAddr.sin_port        = htons(servPort);

        // Connect to the server.
        if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        {
            status = false;
        }
    }

    if (status)
    {
        // Calculate request length.
        requestLen = strlen(request);

        // Send the request to the server.
        if (send(sock, request, requestLen, 0) != requestLen)
        {
            status = false;
        }
    }

    if (status)
    {
        if ((bytesRcvd = recv(sock, recvBuffer, RCVBUFSIZE - 1, 0)) <= 0)
        {
            status = false;
        }

        if (status && (bytesRcvd >0) && (bytesRcvd < (RCVBUFSIZE-1)))
        {
            recvBuffer[bytesRcvd] = '\0';
            CommonLog.Debug("Received buffer=%s", recvBuffer);

            String ipstr(recvBuffer);
            auto pos = ipstr.find_last_of('\n');
            retIp = Ip(ipstr.substr(pos+1));
            CommonLog.Warning("Public IP found to be %s", retIp.ToString().c_str());
        }
    }

    close(sock);

    return status;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ctor
 * @param device
 * @param address
 * @param port
 * @param mcGroup
 * @param mcPort
 * @return 
 */
IpMulticast::IpMulticast(String device, const Ip& address, u16_t port, const Ip& mcGroup, u16_t mcPort)
:   EthHandler("IpMulticast", device, new Ip(address), port, SOCK_DGRAM, IPPROTO_UDP),
    m_mcGroup(new Ip(mcGroup)),
    m_mcPort(mcPort)
{
    AddressToSockAddr(m_mcGroup, mcPort, &m_mcGroupSockAddr);

    // Disable loopback so you do not receive your own datagrams.
    if (m_sockFd != -1)
    {
        char loopch = 0;

        if(setsockopt(m_sockFd, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&loopch, sizeof(loopch)) < 0) {
            m_log.Warning("[%s:%d] %s:%d could not disable loopback, error %d(%s)",
                    m_device.c_str(), m_sockFd, m_mcGroup->ToString().c_str(), mcPort,
                    errno, strerror(errno));
            close(m_sockFd);
            m_sockFd = -1;
        } else
            m_log.Info("[%s:%d] %s:%d disabled loopback",
                        m_device.c_str(), m_sockFd, m_mcGroup->ToString().c_str(), mcPort);
    }

    // Use local interface to drive multicast packets
    if (m_sockFd != -1)
    {
        struct in_addr localInterface;
        //localInterface.s_addr = inet_addr("203.106.93.94");
        // INADDR_ANY
        localInterface.s_addr = (dynamic_cast<Ip*>(m_address))->ToU32();

        if (setsockopt(m_sockFd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localInterface, sizeof(localInterface)) < 0) {
            m_log.Warning("[%s:%d] %s:%d could not add %s:%d, error %d(%s)",
                    m_device.c_str(), m_sockFd, m_mcGroup->ToString().c_str(), mcPort,
                    m_address->ToString().c_str(), port,
                    errno, strerror(errno));
        } else
            m_log.Info("[%s:%d] %s:%d added to %s:%d",
                        m_device.c_str(), m_sockFd, m_mcGroup->ToString().c_str(), mcPort,
                        m_address->ToString().c_str(), port);
    }

    // Add membership to multicast group now
    if (m_sockFd != -1)
    {
        struct ip_mreq group;

        //group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
        //group.imr_interface.s_addr = inet_addr("203.106.93.94");
        group.imr_multiaddr.s_addr = m_mcGroup->ToU32();
        group.imr_interface.s_addr = (dynamic_cast<Ip*>(m_address))->ToU32();

        if (setsockopt(m_sockFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&group, sizeof(group)) < 0) {
            m_log.Warning("[%s:%d] %s:%d could not add on %s:%d, error %d(%s)",
                    m_device.c_str(), m_sockFd,
                    m_mcGroup->ToString().c_str(), mcPort,
                    m_address->ToString().c_str(), port,
                    errno, strerror(errno));
            close(m_sockFd);
            m_sockFd = -1;
        } else
            m_log.Info("[%s:%d] %s:%d added on %s:%d",
                    m_device.c_str(), m_sockFd,
                    m_mcGroup->ToString().c_str(), mcPort,
                    m_address->ToString().c_str(), port);
    }
}

/**
 * @brief dtor
 * @return 
 */
IpMulticast::~IpMulticast()
{
    if (m_sockFd != -1)
    {
        struct ip_mreq group;

        //group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
        //group.imr_interface.s_addr = inet_addr("203.106.93.94");
        group.imr_multiaddr.s_addr = m_mcGroup->ToU32();
        group.imr_interface.s_addr = (dynamic_cast<Ip*>(m_address))->ToU32();

        if (setsockopt(m_sockFd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&group, sizeof(group)) < 0) {
            m_log.Warning("[%s:%d] %s:%d could not remove on %s:%d, error %d(%s)",
                    m_device.c_str(), m_sockFd, m_mcGroup->ToString().c_str(), m_mcPort,
                    m_address->ToString().c_str(), m_port,
                    errno, strerror(errno));
        } else {
            m_log.Info("[%s:%d] %s:%d removed on %s:%d",
                    m_device.c_str(), m_sockFd,
                    m_mcGroup->ToString().c_str(), m_mcPort,
                    m_address->ToString().c_str(), m_port);
        }
    }

    delete m_mcGroup;
    delete m_address;
}

/**
 * @brief Sends packet to multicast group
 * @param packet
 * @param length
 * @return total bytes sent
 */
int IpMulticast::Send(u8_t* packet, int length)
{
    return EthHandler::Send(packet, length, m_mcGroupSockAddr);
}

/**
 * @brief Received packet from multicast group
 * @param packet
 * @param maxLength
 * @param remote
 * @param remotePort
 * @return 
 */
int IpMulticast::ReceiveMc(u8_t* packet, int maxLength, Ip& remote, u16_t& remotePort)
{
    struct sockaddr_in remoteSock;

    int retBytes = Receive(packet, maxLength, &remoteSock);
    SockAddrToIp(&remoteSock, remote, remotePort);

    return retBytes;
}
