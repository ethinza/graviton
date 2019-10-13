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

/**
 * @class Gid
 * @author Chirag
 * @date 06/11/2018
 * @file gid.h
 * @brief GID type class
 */
class Gid : public Address
{
private:

    // Internal representation
    typedef union gid_
    {
        u8_t        raw[(int)Constant::GidByteLength];
        struct
        {
            u64_t   subnet;
            u64_t   interface;
        } global;
        u64_t       u64[2];
        u8_t        u8[(int)Constant::GidByteLength];
    } gid_t;

    gid_t m_value;
    
public:

    /**
     * @brief Default ctor
     * @return 
     */
    Gid()
    :   Address(AddressType::Gid)
    {
        m_value.global.subnet = 0LL;
        m_value.global.interface = 0LL;
    }

    /**
     * @brief Dtor
     * @return 
     */
    ~Gid()
    {
    }

    /**
     * @brief Copy ctor
     * @param right
     */
    Gid(const Gid& right)
    {
        if (&right == *this) return;

        m_value.u64[0] = right.m_value.u64[0];
        m_value.u64[1] = right.m_value.u64[1];
    }

    /**
     * @brief ctor taking string as input
     * @param inputString
     */
    Gid(String inputString)
    {
        char* instr = inputString.c_str();

        strtou64(instr, &(m_value->u64[0]));
        strtou64(&instr[16], &(m_value->u64[1]));
    }

    /**
     * @brief Assignment operator
     * @param right
     */
    Gid& operator = (const Gid& right)
    {
        if (&right == this) return *this;

        m_value.u64[0] = right.m_value.u64[0];
        m_value.u64[1] = right.m_value.u64[1];
        return *this;
    }

    /**
     * @brief Converts content into GID string
     * @return string representing GID value
     */
    String ToString(void)
    {
        char tempString[(int)Constant::GidStringLength + 1] = {0};

        snprintf(tempString, (int)Constant::GuidStringLength, "%016LLX%016LLX",
            m_value.global.subnet, m_value.global.interface);

        return String(tempString);
    }
};
