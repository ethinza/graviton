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

/**
 * @class Address
 * @author Chirag
 * @date 25/11/2018
 * @file address.h
 * @brief Address type base class (abstract)
 */
class Address
{
protected:
    AddressType m_addressType;

public:

    Address() { m_addressType = AddressType::Unknown; }
    Address(AddressType addressType) { m_addressType = addressType; }
    virtual ~Address() {}

    virtual String ToString(void) = 0;
    
    AddressType GetType(void) { return m_addressType; }
};
