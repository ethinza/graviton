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
 * @class Message
 * @author Chirag
 * @date 11/11/2018
 * @file message.h
 * @brief Message base class
 */
class Message
{
private:

    Message(const Message& rhs) = delete;
    Message& operator=(const Message& rhs) = delete;

public:

    Message()
    {
    }

    virtual ~Message()
    {
    }

};

