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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cstdarg>

#include <string>
#include <mutex>
#include <list>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////

#define OUT // parameter for output, returned
#define IN // input parameter
#define INOUT // parameter as input and is modified in return

#ifndef NULL
#define NULL '\0'
#endif

// Widely used basic data types
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long u32_l_t;
typedef unsigned long long u64_t;
typedef unsigned char byte_t;
typedef unsigned short word_t;
typedef unsigned int dword_t;
typedef unsigned long dword_l_t;
typedef unsigned long long qword_t;

// Widely used c++11 data types
typedef std::string String;
typedef std::recursive_mutex Mutex;
typedef std::lock_guard<std::recursive_mutex> LockGuard;
typedef std::list<std::string> StringList;

///////////////////////////////////////////////////////////////////////////////

/**
 * @class Constant
 * @author Chirag
 * @date 04/11/2018
 * @file ethinza.h
 * @brief collection of system wide constants
 */
enum class Constant
{
    // Network layer constants
    IpByteLength            = 4,
    IpStringLength          = 20,
    Ipv6WordLength          = 8,
    Ipv6StringLength        = 40,
    MacByteLength           = 6,
    MacStringLength         = 18,
    GuidByteLength          = 8,
    GuidStringLength        = 20,
    GidByteLength           = 16,
    GidStringLength         = 40,
    NameStringLength        = 32,
    NodeIdByteLength        = 16,
    NodeIdStringLength      = 16,
    TimeStringLength        = 25,

    // OS layer constants
    MemAlignSize            = 64,

    // Application layer constants
    Signature               = 0x0EF4192A,
    ObjectNameLength        = 15,

    // User access constants
    UserIdLength            = 16,
    UserPasswordLength      = 16,
    
    LastConstant
};

// Type of address used
enum class AddressType
{
    Unknown = 0,
    Ipv4 = 1,
    Ipv6 = 2,
    Mac = 3,
    Guid = 4,
    Gid = 5,
    Wwn = 6,

    LastAddressType
};

#define KB (1024)
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)


// Assertion
#define EIZ_ASSERT(condition) \
    if ( !(condition) ) { \
        printf( "ERROR: %s line %d condition \"%s\" failed\n", \
                __FILE__, __LINE__, #condition ); \
        exit( 1 ); \
    }

#define EIZ_ASSERT_WARN(condition) \
    if ( !(condition) ) { \
        printf( "WARNING: %s line %d condition \"%s\" failed\n", \
                __FILE__, __LINE__, #condition ); \
    }

#ifdef __cplusplus
}
#endif
