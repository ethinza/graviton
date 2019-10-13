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
#include <user/userinterface.h>
#include <utility/memoryutility.h>

#include <map>
#include <mutex>

// Bitmask definition for memory features
enum class MemoryFeature
{
    None = 0,
    Aligned = (1 << 1),

    LastMemoryFeature
};

/**
 * @class Memory
 * @author Chirag
 * @date 11/11/2018
 * @file memory.h
 * @brief Memory actions
 */
class Memory
{
private:

    // Definition of memory meta data
    typedef union header_
    {
        u8_t        headerArray[(int)Constant::MemAlignSize];     // 64

        struct
        {
            u32_t   signature;                              // 4
            char    name[(int)Constant::ObjectNameLength + 1];    // +16=20
            u32_t   requestedLength;                        // +4=24
            u32_t   adjustedLength;                         // +4=28
            u32_t   features;                               // +4=32
            u8_t*   object;                                 // +4=36
            u32_t   index;                                  // +4=40
        } component;
    } header_t;

    // Internal memory
    header_t m_internal;
    // Map of objects created
    static std::map<u32_t, Memory*> s_map;
    static std::mutex s_mutex;
    static u32_t s_index;

    Memory();
    Memory(const Memory& rhs) = delete;
    Memory& operator=(const Memory& rhs) = delete;

public:

    Memory(std::string& name, u32_t length, bool isAligned);
    virtual ~Memory();

    // Thread-unsafe getters
    u32_t GetSignature(void) { return m_internal.component.signature; }
    const char* GetName(void) { return m_internal.component.name; }
    u32_t GetLength(void) { return m_internal.component.requestedLength; }
    u32_t GetAllocatedLength(void) { return m_internal.component.adjustedLength; }
    bool IsAligned(void) { return (m_internal.component.features & (u32_t)MemoryFeature::Aligned) != 0; }
    u8_t* GetObject(void) { return m_internal.component.object; }
    u8_t* GetObjectAligned(void) { return MemoryUtility::MemAlignPointer(GetObject()); }
    u32_t GetIndex(void) { return m_internal.component.index; }

    // Dumps memory content
    static void Dump(UserInterface* pui);
};

