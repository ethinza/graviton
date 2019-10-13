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
#include "address.h"

#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

// IP address type
enum class IpAddressType
{
    Unknown = 0,
    Loopback = 1,
    LinkLocal = 2,
    ClassA = 3,
    ClassB = 4,
    ClassC = 5,
    Multicast = 6,

    LastIpAddressType
};

/**
 * @class Ip
 * @author Chirag
 * @date 04/11/2018
 * @file ip.h
 * @brief IP data type class
 */
class Ip : public Address
{
private:
    // Internal representation
    typedef union ip_
    {
        u8_t        u8[(int)Constant::IpByteLength];
        u32_t       u32;
    } ip_t;

    // Base address
    ip_t m_ip;
    // Netmask address
    ip_t m_netmask;
    // Broadcast address
    ip_t m_broadcast;
    // IPv4 address type
    IpAddressType m_ipAddressType;
    // Interface assigned flags, ref http://man7.org/linux/man-pages/man7/netdevice.7.html
    u32_t m_flags;

    // Copies content of right to left
    void Copy(Ip* left, const Ip& right)
    {
        left->m_ip.u32 = right.m_ip.u32;
        left->m_ipAddressType = right.m_ipAddressType;
        left->m_netmask.u32 = right.m_netmask.u32;
        left->m_broadcast.u32 = right.m_broadcast.u32;
        left->m_flags = right.m_flags;
    }

    // Sets IPv4 address type based on value set
    void FindIpAddressType(void)
    {
        if (m_ip.u8[0] == 127) m_ipAddressType = IpAddressType::Loopback;
        else if ((m_ip.u8[0] == 169) && (m_ip.u8[1] == 254)) m_ipAddressType = IpAddressType::LinkLocal;
        else if ((m_ip.u8[0] == 224) && (m_ip.u8[1] == 239)) m_ipAddressType = IpAddressType::Multicast;
        else if (m_ip.u8[0] == 10) m_ipAddressType = IpAddressType::ClassA;
        else if ((m_ip.u8[0] == 172) && ((m_ip.u8[1] & 0xF0) == 0x10)) m_ipAddressType = IpAddressType::ClassB;
        else if ((m_ip.u8[0] == 192) && (m_ip.u8[1] == 168)) m_ipAddressType = IpAddressType::ClassC;
        else m_ipAddressType = IpAddressType::Unknown;
    }

    // Converts string to ip_t base type
    void StrToIpt(const char* in, ip_t& out)
    {
        int idx = 0;
        int len;
        char tmpstr[9];
        size_t maxLen = strlen(in);
        size_t str = 0;

        while (idx < (int)Constant::IpByteLength) {
            memset( tmpstr, 0, sizeof( tmpstr ) );
            len = 0;
            // move until non-numeric character found
            while ((str < maxLen) && isdigit(in[str]) && (len < 8)) {
                tmpstr[len] = in[str];
                ++len;
                ++str;
            }

            out.u8[idx] = (u8_t)strtoul(tmpstr, NULL, 10);
            ++idx;

            // move until numeric character
            while ((str < maxLen) && !isdigit(in[str])) { ++str; }
        }
    }

    // Converts ip_t base type to string
    char* IptToStr(ip_t& in, char* out)
    {
        snprintf(out, (int)Constant::IpStringLength, "%d.%d.%d.%d",
            in.u8[0], in.u8[1], in.u8[2], in.u8[3]);
        return out;
    }

    // Converts sockaddr type to ip_t base
    void SockaddrToIpt(struct sockaddr* in, ip_t& out)
    {
        char ipAddress[INET_ADDRSTRLEN];
        struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(in);

        inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
        StrToIpt(ipAddress, out);
    }

public:

    /**
     * @brief default ctor
     */
    Ip()
    :   Address(AddressType::Ipv4)
    {
        m_ip.u32 = 0;
        m_netmask.u32 = 0;
        m_broadcast.u32 = 0;
        m_ipAddressType = IpAddressType::Unknown;
        m_flags = 0;
    }
    
    Ip(u32_t address)
    :   Address(AddressType::Ipv4)
    {
        m_ip.u32 = address;
        m_netmask.u32 = 0;
        m_broadcast.u32 = 0;
        m_flags = 0;

        FindIpAddressType();
    }

    /**
     * @brief Copy ctor
     * @param right
     */
    Ip(const Ip& right)
    :   Address(AddressType::Ipv4)
    {
        if (&right == this) return;

        Copy(this, right);
    }

    /**
     * @brief ctor taking string as input
     * @param inputString
     */
    Ip(String inputString)
    :   Address(AddressType::Ipv4)
    {
        StrToIpt(inputString.c_str(), m_ip);
        m_netmask.u32 = 0;
        m_broadcast.u32 = 0;
        m_flags = 0;

        FindIpAddressType();
    }

    /**
     * @brief ctor taking ifaddrs as input
     * @param ifa
     * @return 
     */
    Ip(struct ifaddrs* ifa)
    :   Address(AddressType::Ipv4)
    {
        SockaddrToIpt(ifa->ifa_addr, m_ip);
        FindIpAddressType();
        SockaddrToIpt(ifa->ifa_netmask, m_netmask);
        SockaddrToIpt(ifa->ifa_broadaddr, m_broadcast);
        m_flags = ifa->ifa_flags;
    }

    /**
     * @brief dtor
     */
    virtual ~Ip()
    {
    }

    /**
     * @brief Assignment operator
     * @param right
     */
    Ip& operator = (const Ip& right)
    {
        if (&right == this) return *this;

        Copy(this, right);

        return *this;
    }

    /**
     * @brief Converts content into IP string
     * @return string representing IP value
     */
    String ToString(void)
    {
        char tempString[(int)Constant::IpStringLength + 1] = {0};
        return String(IptToStr(m_ip, tempString));
    }

    void ToArray(u8_t* retValue)
    { 
        retValue[0] = m_ip.u8[0];
        retValue[1] = m_ip.u8[1];
        retValue[2] = m_ip.u8[2];
        retValue[3] = m_ip.u8[3];
    }

    u32_t ToU32(void) { return m_ip.u32; }
    IpAddressType GetIpAddressType(void) { return m_ipAddressType; }
    u32_t GetFlags(void) { return m_flags; }

    bool IsActive(void)
    {
        u32_t checkBits = (IFF_UP | IFF_RUNNING | IFF_MULTICAST);
        return (m_flags & checkBits) == checkBits;
    }
};
