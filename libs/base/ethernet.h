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

#include <sys/types.h>
#include <ifaddrs.h>

/**
 * @class Ethernet
 * @author Chirag
 * @date 25/11/2018
 * @file ethernet.h
 * @brief Container class for all ethernet type constants
 */
class Ethernet
{
public:
    // Definition of type of ethernet traffic at data link layer
    enum class DataLinkType
    {
        Unknown = 0,
        Mac = 1,
        Arp = 2,
        Ospf = 3,
        L2tp = 4,
        Ppp = 5,
        Lldp = 6,

        LastDataLinkType
    };

    // Type of ethernet traffic at network layer
    enum class NetworkType
    {
        Unknown = 0,
        Ipv4 = 1,
        Ipv6 = 2,
        Icmp = 3,
        Igmp = 4,
        Ipsec = 5,

        LastNetworkType
    };

    // Type of ethernet traffic at transport layer
    enum class TransportType
    {
        Unknown = 0,
        Raw = 1,
        Udp = 2,
        Tcp = 3,
        Sctp = 4,
        Rsvp = 5,

        LastTransportType
    };

    // Type of ethernet traffic at application layer
    enum class ApplicationType
    {
        Unknown = 0,
        Http = 1,
        Https = 2,
        Ssh = 3,
        Telnet = 4,
        Ftp = 5,
        Snmp = 6,
        Smtp = 7,
        Dhcp = 8,
        Dns = 9,
        Imap = 10,
        Ldap = 11,
        Ntp = 12,
        Pop = 13,
        Scintilla = 14,
        Rtp = 15,
        Tls = 16,
        Storagez = 17,

        LastApplicationType
    };

////////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

    // MAC header in packet
    struct MacHeader
    {
        u8_t            dest_mac[static_cast<int>(Constant::MacByteLength)];
        u8_t            source_mac[static_cast<int>(Constant::MacByteLength)];
        u16_t           ether_type;
    };

    // IP header in packet
    struct IpHeader
    {
        u8_t            version_ihl; // 0-3 version, 4-7 ihl
        u8_t            dscp_ecn; // 0-5 dscp, 6-7 ecn
        u16_t           total_length;

        u16_t           identification;
        u16_t           flags_fragoffset; // 0-2 flags, 3-15 fragment offsest

        u8_t            time_to_live;
        u8_t            protocol;
        u16_t           checksum;

        u32_t           source_addr;

        u32_t           dest_addr;

        u32_t           options[1];
    };

    // IP pseudo header
    struct IpPseudoHeader
    {
        u32_t           source_addr;

        u32_t           dest_addr;

        u8_t            zeros;
        u8_t            protocol;
        u16_t           udp_length;
    };

    // UDP header
    struct UdpHeader
    {
        u16_t           source_port;
        u16_t           dest_port;

        u16_t           length;
        u16_t           checksum;
    };

    // TCP header
    struct TcpHeader
    {
        u16_t           source_port;
        u16_t           dest_port;

        u32_t           sequence_number;

        u32_t           ack_number;

        u16_t           data_off_flags;
        u16_t           window_size;

        u16_t           checksum;
        u16_t           urgent_pointer;

        u32_t           options[1];
    };

#pragma pack(pop)
////////////////////////////////////////////////////////////////////////////////

};

