///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Copyright (c) 2018-2019 Ethinza Inc., All rights reserved                 //
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
#include <base/ethernet.h>

// Multicast group address for discovery
#define MULTICAST_GROUP_STR "233.255.14.7"

/**
 * @class Protocol
 * @author Chirag
 * @date 09/12/2018
 * @file protocol.h
 * @brief Core protocol definition for storageZ
 */
class Protocol
{
public:

    // Protocol constants
    enum class Constant
    {
        Signature = 0x0EF4192A,
        Version = 100,
        Port = 19007,

        MaxPacketSize = 1400,

        LastConstant
    };

    // Header type which reflects request/response mechanism
    enum class PacketType
    {
        Undefined = 0, // undefined
        Request = 1, // request packet which needs a response from recipient
        Response = 2, // response packet to an already received request
        Ack = 3, // acknowledgement packet which allows responder to delay sending response
        Notification = 4, // notification packet which does not require acknowledgement nor response

        LastPacketType
    };

    // Packet identifier definition
    enum class PacketId
    {
        Registration = 0, // Registration packets

        LastPacketId
    };

    // Type of node initiating the packet
    enum class NodeType
    {
        Unknown = 0, // unknown
        EthSwitch = 1, // Ethernet switch
        EthStorage = 2, // Ethernet storage
        EthUser = 3, // Ethernet user
        EthScb = 4, // Ethernet scb (super-converged bridge)
        IbSwitch = 5, // Infiniband switch
        IbStorage = 6, // Infiniband storage
        IbUser = 7, // Infiniband user
        IbScb = 8, // Infiniband scb (super-converged bridge)
        NvmeStorage = 9, // NVMe storage
        FcSwitch = 10, // Fibre Channel switch
        FcStorage = 11, // Fibre Channel storage
        FcUser = 12, // Fibre Channel user
        FcScb = 13, // Fibre Channel scb (super-converged bridge)
        BackupUser = 14, // Data backup user
        DrUser = 15, // Disaster recovery (offsite backup) user
        SwitchZ = 16,
        StoreZ = 17,
        UserZ = 18,

        LastNodeType
    };

    // Type of application
    enum class AppType
    {
        Undefined = 0,
        Centos = 1,
        Ios = 2,
        Android = 3,
        WinPhone = 4,
        WebBrowser = 5,
        ThirdParty = 6,
        Windows = 7,
    
        LastAppType
    };

    // Application features
    enum class Feature
    {
        Undefined = 0,
        Backup = (1 << 1),
        Dr = (1 << 2),
        Encryption = (1 << 3),
        Dedup = (1 << 4),

        LastFeature
    };

    // Interface type
    enum class InterfaceType
    {
        Unknown = 0,
        Ipv4 = 1,
        Ipv6 = 2,
        Ib = 2,
        Fc = 3,

        LastInterfaceType
    };

    // One or more capabilities of interface
    // Common capabilities are unique bit positions
    // Specific capabilities can use the same bit postions
    enum class InterfaceCapability
    {
        // Common
        Undefined = 0,
        Lan = (1 << 1),
        Isl = (1 << 2),
        Wan = (1 << 3),

        // Ethernet specific
        Ethernet1g = (1 << 16),
        Ethernet10g = (1 << 17),
        Ethernet40g = (1 << 18),
        Ethernet100g = (1 << 19),

        // Infiniband specific
        IbDdr = (1 << 16), // 5gbps signaling speed
        IbQdr = (1 << 17), // 10gbps
        IbFdr10 = (1 << 18), // 10gbps
        IbFdr = (1 << 19), // 14gbps
        IbEdr = (1 << 20), // 25gbps
        IbHdr = (1 << 21), // 50gbps
        IbNdr = (1 << 22), // 100gbps
        IbXdr = (1 << 23), // 250gbps
        Ib1x = (1 << 24), // signaling speed x 1
        Ib4x = (1 << 25), // signaling speed x 4
        Ib8x = (1 << 26), // signaling speed x 8
        Ib12x = (1 << 27), // signaling speed x 12
        Ib16x = (1 << 28), // signaling speed x 16

        LastInterfaceCapability
    };

////////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

    // Header definition for all types of packets
    struct Header
    {
        u32_t   signature;

        u16_t   sequence; // unique sequence number if relevant
        u8_t    packet_type; // enum PacketType
        u8_t    retry_count; // retry count for the same sequence if applicable

        u16_t   length; // length of data
        u8_t    version; // protocol version
        u8_t    packet_id; // enum PacketId

        u8_t    node_type; // enum NodeType
        u16_t   node_id; // unique node id of the sender
        u8_t    reserved; // future use

        u32_t   reserved2; // future use
    };

    // Interface attributes
    struct Interface
    {
        u8_t    type; // enum InterfaceType
        u8_t    reserved[3];

        u32_t   cap_bits; // one or more of enum InterfaceCapability bits

        u8_t    address[16]; // depends on InterfaceType
    };

    // Registration packet, also used as heartbeat
    struct Registration
    {
        struct Header   header; // packet_id=1

        u16_t   app_version; // application version, major << 8 + minor
        u8_t    app_type; // enum AppType
        u8_t    reserved1; // future use

        u8_t    public_ip[4]; // public IP address
        u32_t   feature_bits; // one or more enum Feature bits
        u64_t   storage_size; // total storage available

        u8_t    total_interfaces; // total connections in node
        u8_t    reserved2[3]; // future use

        struct Interface    interfaces[1]; // Connection attributes
    };

#pragma pack(pop)
////////////////////////////////////////////////////////////////////////////////

private:

    // Current sequence counter
    u16_t m_sequence;
    // Unique node id
    u16_t m_nodeId;
    // Node type
    NodeType m_nodeType;
    // Application type
    AppType m_appType;
    // Application version value
    u16_t m_appVersion;

    // default ctor
    Protocol()
    :   m_sequence(0),
        m_nodeId(0),
        m_nodeType(NodeType::Unknown),
        m_appType(AppType::Undefined),
        m_appVersion(0)
    {
    }

    // dtor
    ~Protocol()
    {
    }

    Protocol(const Protocol& right) = delete;
    Protocol& operator = (const Protocol& right) = delete;

public:

    // Returns signleton object
    static Protocol& GetInstance(void)
    {
        static Protocol protocol;
        return protocol;
    }

    u8_t GetProtocolVersion(void) { return static_cast<u8_t>(Constant::Version); }
    u16_t GetNextSequence(void) { return ++m_sequence; }
    void SetNodeId(u16_t nodeId) { m_nodeId = nodeId; }
    u16_t GetNodeId(void) { return m_nodeId; }
    void SetNodeType(NodeType nodeType) { m_nodeType = nodeType; }
    NodeType GetNodeType(void) { return m_nodeType; }
    void SetAppType(AppType appType) { m_appType = appType; }
    AppType GetAppType(void) { return m_appType; }
    void SetAppVersion(u8_t major, u8_t minor) { m_appVersion = (static_cast<u16_t>(major) << 8) + minor; }
    u16_t GetAppVersion(void) { return m_appVersion; }

    void PrepareHeader(struct Header& header,
                            PacketId packetId,
                            PacketType packetType,
                            u16_t sequence,
                            u8_t retry,
                            u16_t packetLength);
};

#define ProtoInst Protocol::GetInstance()

// Union of all packets
union Packet
{
    u8_t scratchPad[static_cast<int>(Protocol::Constant::MaxPacketSize)];

    Protocol::Registration registration;
};
