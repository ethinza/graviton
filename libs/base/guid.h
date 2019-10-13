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
 * @class Guid
 * @author Chirag
 * @date 06/11/2018
 * @file guid.h
 * @brief GUID type class
 */
class Guid : public Address
{
private:

    // Internal representation
    typedef union guid_
    {
        u8_t        u8[(int)Constant::GuidByteLength];
        u64_t       u64;
    } guid_t;

    guid_t m_value;
    
public:

    /**
     * @brief Default ctor
     * @return 
     */
    Guid()
    :   Address(AddressType::Guid)
    {
        m_value.u64 = 0LL;
    }

    /**
     * @brief Dtor
     * @return 
     */
    ~Guid()
    {
    }

    /**
     * @brief Copy ctor
     * @param right
     */
    Guid(const Guid& right)
    {
        if (&right == this) return;

        m_value.u64 = right.m_value.u64;
    }

    /**
     * @brief ctor taking string as input
     * @param inputString
     */
    Guid(String inputString)
    {
        strtou64(inputString.c_str(), &(m_value->u64));
    }

    /**
     * @brief Assignment operator
     * @param right
     */
    Guid& operator = (const Guid& right)
    {
        if (&right == this) return *this;

        m_value.u64 = right.m_value.u64;
        return *this;
    }

    /**
     * @brief Converts content into GUID string
     * @return string representing GUID value
     */
    String ToString(void)
    {
        char tempString[(int)Constant::GuidStringLength + 1] = {0};

        snprintf(tempString, (int)Constant::GuidStringLength, "%016llX",
            m_value.u64);

        return String(tempString);
    }
};

