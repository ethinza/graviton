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

#include "memory.h"

// Initializing statics
u32_t Memory::s_index = 0;
std::map<u32_t, Memory*> Memory::s_map;
std::mutex Memory::s_mutex;

/**
 * @brief Default ctor
 * @return 
 */
Memory::Memory()
{
    memset(&m_internal, 0, sizeof(m_internal));
}

/**
 * @brief User callable ctor
 * @param name max characters Constants.ObjectNameLength - 1
 * @param length requested length of bytes
 * @param isAligned memory is allocated aligned if true
 */
Memory::Memory(std::string& name, u32_t length, bool isAligned)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    m_internal.component.signature = (u32_t)Constant::Signature;
    m_internal.component.index = ++s_index;
    strncpy(m_internal.component.name, name.c_str(), (int)Constant::ObjectNameLength);
    m_internal.component.requestedLength = length;
    m_internal.component.adjustedLength = isAligned ? MemoryUtility::MemAlignRoundedUpExtra(length) : length;
    m_internal.component.features = isAligned ? (u32_t)MemoryFeature::Aligned : (u32_t)MemoryFeature::None;
    m_internal.component.object = new u8_t[m_internal.component.adjustedLength];
    
    s_map[m_internal.component.index] = this;
}

/**
 * @brief dtor
 */
Memory::~Memory()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    
    s_map.erase(m_internal.component.index);
    delete[] m_internal.component.object;
}

/**
 * @brief Dumps tracked memory content to user interface output method
 *      This is a lengthy operation and whole operation locks the content
 * @param ui requesting object
 */
void Memory::Dump(UserInterface* ui)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    ui->Output("--------|--------|--------|--------|----------------|-|--------\n" );
    ui->Output("Index   |Features|ReqLen  |AdjLen  |Name            |X|Object\n" );
    ui->Output("--------|--------|--------|--------|----------------|-|--------\n" );

    for (std::map<u32_t, Memory*>::iterator it = s_map.begin(); it != s_map.end(); ++it)
    {
        Memory* memory = it->second;
        ui->Output("%8d %08x %8d %8d %-16.16s %c %08x\n",
                    memory->m_internal.component.index,
                    memory->m_internal.component.features,
                    memory->m_internal.component.requestedLength,
                    memory->m_internal.component.adjustedLength,
                    memory->m_internal.component.name,
                    memory->m_internal.component.signature == (u32_t)Constant::Signature ? ' ' : 'X',
                    memory->m_internal.component.object);
    }

    ui->Output("--------|--------|--------|--------|----------------|-|--------\n" );
}
