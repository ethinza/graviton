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
 * @class UserInterface
 * @author Chirag
 * @date 11/11/2018
 * @file userinterface.h
 * @brief User interface base class
 */
class UserInterface
{
private:

    UserInterface(const UserInterface& rhs) = delete;
    UserInterface& operator=(const UserInterface& rhs) = delete;

public:

    /**
     * @brief Default ctor
     * @return 
     */
    UserInterface()
    {
    }

    /**
     * @brief Dtor
     * @return 
     */
    virtual ~UserInterface()
    {
    }

    // abstract function to write output string to
    virtual void Output(const char* output, ...) = 0;
};

