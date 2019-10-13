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
#include <utility/osinterface.h>

/**
 * @class MemoryUtility
 * @author Chirag
 * @date 11/11/2018
 * @file memoryutility.h
 * @brief Memory operations utility class
 */
class MemoryUtility
{
private:

    MemoryUtility() {}
    ~MemoryUtility() {}
    MemoryUtility(const MemoryUtility& rhs) = delete;
    MemoryUtility& operator=(const MemoryUtility& rhs) = delete;

public:


    /**
     * @brief Gets rounded up value for a given size
     * @param desiredSize in bytes
     * @return Rounded up value in bytes
     */
    static u32_t MemAlignRoundup(u32_t desiredSize)
    {
        return (desiredSize + (u32_t)Constant::MemAlignSize) & ~((u32_t)Constant::MemAlignSize - 1);
    }

    /**
     * @brief Converts object memory pointer to aligned memory pointer
     * @param object
     * @return aligned object
     */
    static u8_t* MemAlignPointer(u8_t* object)
    {
        return (u8_t*)(((u64_t)object + ((u32_t)Constant::MemAlignSize * 2) - 1) & ~((u32_t)Constant::MemAlignSize - 1));
    }

    /**
     * @brief Gets allocation length for a given size that includes one extra
     *          block of align size
     * @param desiredSize in bytes
     * @return memory allocation size in bytes
     */
    static u32_t MemAlignRoundedUpExtra(u32_t desiredSize)
    {
        return MemAlignRoundup(desiredSize) + (u32_t)Constant::MemAlignSize;
    }

    /**
     * @brief Swaps 8bit value just to satisfy overloaded APIs
     * @param value
     * @return swapped value
     */
    static u8_t Swap(u8_t value) { return value; }
    static u8_t CheckAndSwap(u8_t value) { return value; }

    /**
     * @brief Swaps 16bit value
     * @param value
     * @return swapped value
     */
    static u16_t Swap(u16_t value)
    {
        return (((value & 0xff) << 8) | ((value >> 8) & 0xff));
    }

    /**
     * @brief Swaps 16bit value on little endian machine
     * @param value
     * @return swapped value
     */
    static u16_t CheckAndSwap(u16_t value)
    {
        return Osi.IsBigEndianMachine() ? value : Swap(value);
    }

    /**
     * @brief Swaps 32bit value
     * @param value
     * @return swapped value
     */
    static u32_t Swap(u32_t value)
    {
        return ( ( ( value & 0xff ) << 24 )
                | ( ( value >> 24 ) & 0xff )
                | ( ( value & 0xff00 ) << 8 )
                | ( ( value >> 8 ) & 0xff00 ) );
    }

    /**
     * @brief Swaps 32bit value on little endian machine
     * @param value
     * @return swapped value
     */
    static u32_t CheckAndSwap(u32_t value)
    {
        return Osi.IsBigEndianMachine() ? value : Swap(value);
    }

    /**
     * @brief Swaps 64bit value
     * @param value
     * @return swapped value
     */
    static u64_t Swap(u64_t value)
    {
        return ( ( ( value & 0xffLL ) << 56 )
                | ( ( value >> 56 ) & 0xffLL )
                | ( ( value & 0xff00LL ) << 40 )
                | ( ( value >> 40 ) & 0xff00LL )
                | ( ( value & 0xff0000LL ) << 24 )
                | ( ( value >> 24 ) & 0xff0000LL )
                | ( ( value & 0xff000000LL ) << 8 )
                | ( ( value >> 8 ) & 0xff000000LL ) );
    }

    /**
     * @brief Swaps 64bit value on little endian machine
     * @param value
     * @return swapped value
     */
    static u64_t CheckAndSwap(u64_t value)
    {
        return Osi.IsBigEndianMachine() ? value : Swap(value);
    }
};

