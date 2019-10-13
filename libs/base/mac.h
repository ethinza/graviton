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
#include "address.h"

#include <netpacket/packet.h>

/**
 * @class Mac
 * @author Chirag
 * @date 06/11/2018
 * @file mac.h
 * @brief MAC data type class
 */
class Mac : public Address
{
private:

    // Internal representation
    typedef union mac_
    {
        u8_t        u8[static_cast<int>(Constant::MacByteLength)];
        u64_t       u64;
    } mac_t;

    mac_t m_value;

public:

    /**
     * @brief Default ctor
     * @return 
     */
    Mac()
    :   Address(AddressType::Mac)
    {
        m_value.u64 = 0LL;
    }

    /**
     * @brief Copy ctor
     * @param right
     * @return 
     */
    Mac(const Mac& right)
    :   Address(AddressType::Mac)
    {
        if (&right == this) return;

        m_value.u64 = right.m_value.u64;
    }

    /**
     * @brief ctor taking string as input
     * @param inputString
     */
    Mac(String inputString)
    :   Address(AddressType::Mac)
    {
        int idx = 0;
        int len;
        char tmpstr[8] = {0};
        char* instr = const_cast<char*>(inputString.c_str());

        while ( idx < (int)Constant::MacByteLength ) {
            memset(tmpstr, 0, sizeof(tmpstr));
            len = 0;
            // move until non-numeric character found
            while (*instr && isxdigit(*instr)) {
                tmpstr[len] = *instr;
                ++len;
                ++instr;
            }

            m_value.u8[idx] = (u8_t)strtoul(tmpstr, NULL, 16);
            ++idx;
            // move until numeric character
            while (*instr && !isxdigit(*instr)) {
                ++instr;
            }
        } // while
    }

    /**
     * @brief ctor taking ifaddrs as input
     * @param ifa
     * @return 
     */
    Mac(struct ifaddrs* ifa)
    :   Address(AddressType::Mac)
    {
        struct sockaddr_ll* s = reinterpret_cast<struct sockaddr_ll*>(ifa->ifa_addr);

        for (int i = 0; i < s->sll_halen; i++) {
            if (i < static_cast<int>(Constant::MacByteLength)) m_value.u8[i] = s->sll_addr[i];
        }
    }

    /**
     * @brief Dtor
     * @return 
     */
    virtual ~Mac()
    {
    }

    /**
     * @brief Assignment operator
     * @param right
     */
    Mac& operator = (const Mac& right)
    {
        if (&right == this) return *this;

        m_value.u64 = right.m_value.u64;
        return *this;
    }

    /**
     * @brief Converts content into MAC string
     * @return string representing MAC value
     */
    String ToString(void)
    {
        char tempString[(int)Constant::MacStringLength + 1] = {0};

        snprintf(tempString, (int)Constant::MacStringLength,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            m_value.u8[0], m_value.u8[1], m_value.u8[2], m_value.u8[3],
            m_value.u8[4], m_value.u8[5]);

        return String(tempString);
    }
    
    u64_t ToU64(void) { return m_value.u64; }
    bool IsUnicast(void) { return (m_value.u8[0] & 0x1) == 0; }
    bool IsMulticast(void) { return (m_value.u8[0] & 0x1) == 1; }
    bool IsLocal(void) { return (m_value.u8[0] & 0x2) == 0x2; }
    bool IsGlobal(void) { return (m_value.u8[0] & 0x2) == 0; }

    bool IsBroadcast(void)
    {
        return (m_value.u8[0] == 0xff)
            && (m_value.u8[1] == 0xff)
            && (m_value.u8[2] == 0xff)
            && (m_value.u8[3] == 0xff)
            && (m_value.u8[4] == 0xff)
            && (m_value.u8[5] == 0xff);
    }

    u32_t GetOui(void)
    {
        return (static_cast<u32_t>(m_value.u8[0]) << 16)
            || (static_cast<u32_t>(m_value.u8[1]) << 8)
            || static_cast<u32_t>(m_value.u8[2]);
    }
};

