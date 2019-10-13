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

/**
 * Type of node based on functionality
 */
enum class Functionality
{
    Undefined       = 0,

    Switch          = 1,
    Compute         = 2,
    Gateway         = 3,
    Storage         = 4,
    ComputeData     = 5,
    NetworkLink     = 6, // Inter-network link
    LocationLink    = 7, // WAN link

    LastFunctionality
};

/**
 * Type of node based on physical connectivity/storage
 */
enum class Connection
{
    Unknown         = 0,

    Ethernet        = 1,
    Infiniband      = 2,
    FibreChannel    = 3,
    Usb             = 4,
    Pci             = 5,
    Nvme            = 6,

    LastConnection
};

/**
 * Type of device for a node
 */
enum class Device
{
    Unknown         = 0,

    Ssd             = 1
    Disk            = 2,
    Jbod            = 3,
    Raid            = 4,
    NetworkDisk     = 5,

    LastDevice
};

/**
 * @class NodeId
 * @author Chirag
 * @date 10/11/2018
 * @file nodeid.h
 * @brief Unique identifier for every node in network
 */
class NodeId
{
private:

    // Internal representation
    typedef union nodeid_
    {
        u64_t       u64[2];
        u8_t        u8[(int)Constant::NodeIdByteLength];

        struct x
        {
            u8_t    functionality; // Functionality
            u8_t    connection; // Connection
            u8_t    device; // Device
            u8_t    location; // unique location number

            u16_t   network; // unique network number within location
            u16_t   node; // unique node number
        } component;
    } nodeid_t;
    
    nodeid_t m_value;

public:

    // Default ctor
    NodeId()
    {
        m_value = 0;
    }

    // Copy ctor
    NodeId(Const NodeId& right)
    {
        if (&right == this) return;
        
        m_value.u64[0] = right.m_value.u64[0];
        m_value.u64[1] = right.m_value.u64[1];
    }

    // Dtor
    virtual ~NodeId()
    {
    }

    // Assignment operator
    NodeId& operator = (const NodeId& right)
    {
        if (&right == this) return *this;

        m_value.u64[0] = right.m_value.u64[0];
        m_value.u64[1] = right.m_value.u64[1];
        return *this;
    }

    // Converts content into string
    String ToString(void)
    {
        char tempString[(int)Constant::NodeIdStringLength + 1] = {0};

        snprintf(tempString, (int)Constant::NodeIdStringLength, "%02x:%04x:%04x-%d-%d-%d",
            m_value.component.location, m_value.component.network,
            m_value.component.node, m_value.component.functionality,
            m_value.component.connection, m_value.component.device);

        return String(tempString);
    }
};

