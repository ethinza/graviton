///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019 Ethinza Inc, All rights reserved
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

class StringBuilder
{
private:
    String m_text;

    StringBuilder(const StringBuilder& rhs) = delete;
    StringBuilder& operator=(const StringBuilder& rhs) = delete;

public:
    StringBuilder();
    virtual ~StringBuilder();

    const char* GetText()
    {
        return m_text.c_str();
    }

    operator const char*()
    {
        return GetText();
    }

    int Length() const
    {
        return m_text.length();
    }

    void AppendLine(const char* format, ...);
    void Append(const char* format, ...);
    void AppendKeyVal(const char* format, uint8_t* val, int length);

    void Clear();
};
