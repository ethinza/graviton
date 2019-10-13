///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2018-2019 Ethinza Inc, All rights reserved
//
// This file contains TRADE SECRETs of ETHINZA INC and is CONFIDENTIAL.
// Without written consent from ETHINZA INC, it is prohibited to disclose or
// reproduce its contents, or to manufacture, use or sell it in whole or part.
// Any reproduction of this file without written consent of ETHINZA INC is a 
// violation of the copyright laws and is subject to civil liability and 
// criminal prosecution.
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <ethinza.h>
#include <list>

typedef void (*AddCommandFunction) (String, String, String);

/**
 * @class StringUtility
 * @author Chirag
 * @date 02/12/2018
 * @file stringutility.h
 * @brief Various string utility APIs
 */
class StringUtility
{
private:

    StringUtility() {}
    ~StringUtility() {}
    StringUtility(const StringUtility& rhs) = delete;
    StringUtility& operator=(const StringUtility& rhs) = delete;

    static String InterpretIniSection(String& inputString);
    static bool InterpretIniKeyValue(String& inputString, String& key, String& value);

public:

    static char* Trim(char* instr);
    static bool IsComment(const char* instr);
    static char* ToLower(char* instr);
    static void InterpretIniFile(String fileName, AddCommandFunction addCommandFunction);
    static String Format(const char* format, va_list args);
    static String Format(const char* format, ...);
    static int Split(char* inString, const char* delimiter, StringList& output);
    static int SplitIpv4(const char* ipAddresses, StringList& output);
};

