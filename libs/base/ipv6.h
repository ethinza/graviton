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
 * @class Ipv6
 * @author Chirag
 * @date 04/11/2018
 * @file ipv6.h
 * @brief IPv6 data type class
 */
class Ipv6 : public Address
{
private:

    // Internal representation
    typedef union ipv6_
    {
        u8_t        u8[(int)Constant::Ipv6WordLength * 2];
        u16_t       u16[(int)Constant::Ipv6WordLength];
    } ipv6_t;

    ipv6_t m_value;

public:

    /**
     * @brief Default ctor
     */
    Ipv6()
    :   Address(AddressType::Ipv6)
    {
        for (int idx = 0; idx < (int)Constant::Ipv6WordLength; ++idx)
            m_value.u16[idx] = 0;
    }

    /**
     * @brief Copy ctor
     * @param right
     * @return 
     */
    Ipv6(const Ipv6& right)
    :   Address(AddressType::Ipv6)
    {
        if (&right == this) return;

        for (int idx = 0; idx < (int)Constant::Ipv6WordLength; ++idx)
            m_value.u16[idx] = right.m_value.u16[idx];
    }

    /**
     * @brief ctor taking string as input
     * @param inputString
     */
    Ipv6(String inputString)
    :   Address(AddressType::Ipv6)
    {
        char tmpstr[8] = {0};
        int len = 0;
        int idx = 0;
        bool toConvert = true;
        char* instr = const_cast<char*>(inputString.c_str());

        while (idx < (int)Constant::Ipv6WordLength) {
            if (toConvert) {
                memset(tmpstr, 0, sizeof(tmpstr));
                len = 0;

                while (*instr && isxdigit(*instr)) {
                    tmpstr[len] = *instr;
                    ++len;
                    ++instr;
                }

                m_value.u16[idx] = (u16_t)strtoul(tmpstr, NULL, 16);
                ++idx;
            } else {
                m_value.u16[idx] = 0;
                ++idx;
                toConvert = true;
                --instr;
            }

            if (*instr && *instr == ':') {
                ++instr;
                if (*instr && *instr == ':') {
                    ++instr;
                    toConvert = false;
                } else
                    toConvert = true;
            }
        } // while
    }

    /**
     * @brief Assignment operator
     * @param right
     * @return this
     */
    Ipv6& operator = (const Ipv6& right)
    {
        if (&right == this) return *this;

        for (int idx = 0; idx < (int)Constant::Ipv6WordLength; ++idx)
            m_value.u16[idx] = right.m_value.u16[idx];

        return *this;
    }

    /**
     * @brief Virtual dtor
     */
    virtual ~Ipv6()
    {
    }

    /**
     * @brief Converts content into IPv6 string
     * @return string representing IPv6 value
     */
    String ToString(void)
    {
        char tempString[((int)Constant::Ipv6WordLength * 2) + 1] = {0};

        snprintf(tempString, (int)Constant::Ipv6WordLength * 2,
            "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
            m_value.u16[0], m_value.u16[1], m_value.u16[2], m_value.u16[3],
            m_value.u16[4], m_value.u16[5], m_value.u16[6], m_value.u16[7]);

        return String(tempString);
    }
};
