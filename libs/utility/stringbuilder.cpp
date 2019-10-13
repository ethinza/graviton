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

#include "stringutility.h"
#include "stringbuilder.h"

StringBuilder::StringBuilder()
{
}


StringBuilder::~StringBuilder()
{
}

void StringBuilder::Append(const char* format, ...)
{
    va_list args;
    String text;

    va_start(args, format);
    text = StringUtility::Format(format, args);
    va_end(args);
    m_text.append(text);
}

void StringBuilder::AppendLine(const char* format, ...)
{
    va_list args;
    String text;

    va_start(args, format);
    text = StringUtility::Format(format, args);
    va_end(args);
    m_text.append(text);
    m_text.append("\n");
}

void StringBuilder::Clear()
{
    m_text.clear();
}