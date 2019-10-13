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
#include "stringutility.h"
#include <service/logmanager.h>
#include <base/ip.h>

#include <fstream>
#include <cstring>

typedef char TStringBuffer[4 * 1024];
static thread_local TStringBuffer s_stringBuilder;

// removes leading and trailing white spaces characters from string
// Input string must be NULL terminated and will be altered.
char* StringUtility::Trim(char* instr)
{
    size_t len = strlen(instr);
    size_t idx, newIdx;
    bool firstNonSpace = false;
    size_t lastSpace = 0;

    for ( idx = 0, newIdx = 0; idx < len; ++idx )
    {
        if ( !firstNonSpace &&
            ( ( isspace( instr[ idx ] ) ||
              ( instr[ idx ] == 0 ) ) ||
              //( instr[ idx ] == '\r' ) ||
              ( instr[ idx ] == '\n' ) )
            )
            continue;

        if ( ( instr[ idx ] == ';' ) ||
            ( strncmp( &instr[ idx ], "//", 2 ) == 0 )
            )
        {
            instr[ lastSpace ] = '\0';
            break; // remove comment after code characters
        }

        if ( ( instr[ idx ] == ' ' ) ||
            ( instr[ idx ] == '\t' )
            )
            lastSpace = newIdx;

        firstNonSpace = true;
        instr[ newIdx ] = instr[ idx ];
        ++newIdx;
    } // for, len

    instr[ newIdx ] = '\0';

    return instr;
}

// Returns non-zero value if comment string is found
bool StringUtility::IsComment(const char* instr)
{
    if ( instr == nullptr ||
        instr[0] == ';' ||
        instr[0] == '/' ||
        instr[0] == 0x0d ||
        instr[0] == 0x0a ||
        instr[0] == 0
        )
        return true;

    return false;
}

// Converts characters from string to lowercase
// Input string must be NULL terminated and will be altered.
char* StringUtility::ToLower(char* instr)
{
    size_t len = strlen( instr );
    size_t idx;

    for ( idx = 0; idx < len; ++idx ) {
        instr[ idx ] = ( char )tolower( instr[ idx ] );
    } // for, len

    return instr;
}

/**
 * @brief Parses INI type file and calls function to add section-key-value
 * @param fileName
 * @param addCommandFunction
 */
void StringUtility::InterpretIniFile(String fileName, AddCommandFunction addCommandFunction)
{
    String section = "";
    bool multilineStart = false;
    String multiline = "";
    String key = "";
    char line[128];

    std::ifstream ifs(fileName);

    while (ifs.good())
    {
        ifs.getline(line, 127);
        String trimmedLine(StringUtility::Trim(line));

        if (StringUtility::IsComment(trimmedLine.c_str())) continue;
        CommonLog.Info("TRIMMED---%s---", trimmedLine.c_str());

        if (multilineStart)
        {
            if (trimmedLine.find_first_of(']') != String::npos)
            {
                (*addCommandFunction)(section, key, multiline);

                // reset multiline for next
                multilineStart = false;
                multiline = "";
            }
            else
            {
                multiline.append(trimmedLine);
            }

            continue;
        }

        String tempSection = InterpretIniSection(trimmedLine);

        if (tempSection.empty())
        {
            String value;

            if (InterpretIniKeyValue(trimmedLine, key, value))
            {
                if (value.length() > 0)
                {
                    if (value[0] == '[')
                    {
                        multilineStart = true;
                        continue;
                    }
                }

                (*addCommandFunction)(section, key, value);
            }
        }
        else
        {
            section = tempSection;
        }
    }

    ifs.close();
}

// Interprets section part from input string
String StringUtility::InterpretIniSection(String& inputString)
{
    int sectionStart = inputString.find_first_of('[');
    int sectionEnd = inputString.find_first_of(']');

    if ((sectionStart >= 0) && (sectionEnd >= 1))
    {
        return inputString.substr(sectionStart + 1, sectionEnd - 1);
    }
    else
    {
        return "";
    }
}

// Interprets content as key and value pair
// Does not take care of multiline type values.
bool StringUtility::InterpretIniKeyValue(String& inputString, String& key, String& value)
{
    int separatorIndex = inputString.find_first_of('=');
    int endIndex = inputString.find_first_of(';');

    if (separatorIndex > 1)
    {
        key = inputString.substr(0, separatorIndex);
        value = endIndex > 1
                    ? inputString.substr(separatorIndex + 1, endIndex)
                    : inputString.substr(separatorIndex + 1);

        return true;
    }
    else
    {
        return false;
    }
}

String StringUtility::Format(const char* format, va_list args)
{
    try
    {
        //If needed - use this mechanism to determine the size of the resulting buffer
        //int size = vsnprintf(nullptr, 0, format, args) + sizeof('\0');
        //char buffer[size];

        int rc = vsnprintf(s_stringBuilder, sizeof(s_stringBuilder), format, args);

        if (rc < 0)
        {
            sprintf(s_stringBuilder, "Error formatting string '%s'.", format);
        }
        else if (static_cast<size_t>(rc) >= sizeof(s_stringBuilder))
        {
            // string truncated...
//            string text(s_stringBuilder, 20);
//            text += "...";
//            CommonLog.Warning("String %s truncated to %d length.", text.c_str(), rc);
        }
    }
    catch (...)
    {
        sprintf(s_stringBuilder, "Exception formatting string '%s'.", format);
    }

    return s_stringBuilder;
}

String StringUtility::Format(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    String str = Format(format, args);
    va_end(args);

    return str;
}

/**
 * @brief Splits input string into list separated by delimiter
 * @param inString string to split
 * @param delimiter character(s)
 * @param output list of strings
 * @return number of strings in output list
 */
int StringUtility::Split(char* inString, const char* delimiter, StringList& output)
{
    char* token;
    char* brk;
    int count = 0;

    token = strtok_r(inString, delimiter, &brk);

    while (token != NULL) {
        output.push_back(String(token));
        token = strtok_r(NULL, delimiter, &brk);
        ++count;
    }

    return count;
}

/**
 * @brief Splits ipv4 input addresses into list
 * @param ipAddresses string to split
 * @param output list of ipaddresses
 * @return number of ipv4 addresses found
 */
int StringUtility::SplitIpv4(const char* ipAddresses, StringList& output)
{
    char tmpIpAddresses[1024] = {0};
    int count = 0;

    strncpy(tmpIpAddresses, ipAddresses, strlen(ipAddresses));
    output.clear();

    // check for array split first e.g. 192.168.1.5-10
    if (strchr(tmpIpAddresses, '-') != NULL)
    {
        StringList ipList;

        StringUtility::Split(tmpIpAddresses, "-", ipList);

        StringList::iterator it = ipList.begin();
        Ip ip(*it);
        u8_t startIp[(int)Constant::IpByteLength];
        ip.ToArray(startIp);

        ++it;
        u8_t end = (u8_t)strtoul(it->c_str(), NULL, 10);

        for (u8_t idx = startIp[3]; idx <= end; ++idx)
        {
            output.push_back(StringUtility::Format("%d.%d.%d.%d", startIp[0], startIp[1], startIp[2], idx));
            ++count;
        }
    }
    else
    {
        count = StringUtility::Split(tmpIpAddresses, ",", output);
    }

    return count;
}

/*
// Returns non-zero if all numeric characters are found in NULL terminated string
int isnumeric( const char* instr )
{
    size_t len = strlen( instr );
    size_t idx;

    for ( idx = 0; idx < len; ++idx ) {
        if ( ( idx == 0 ) && ( instr[0] == '0' ) )
            continue;
        if ( ( idx == 1 ) && ( ( instr[1] == 'x' ) || ( instr[1] == 'X' ) ) )
            continue;
        if ( isxdigit( instr[ idx ] ) == 0 )
            return 0;
    } // for, len

    return 1;
}

// Finds first non-space character and returns its pointer
// Input string must be NULL terminated.
char* strnonspace( char* instr )
{
    size_t len = strlen( instr );
    size_t idx;

    for ( idx = 0; idx < len; ++idx ) {
        if ( isspace( instr[ idx ] ) )
            continue;
        return &instr[ idx ];
    } // for, len

    return NULL;
}

// Finds first space character and returns its pointer
// Input string must be NULL terminated.
char* strspace( char* instr )
{
    size_t len = strlen( instr );
    size_t idx;

    for ( idx = 0; idx < len; ++idx ) {
        if ( isspace( instr[ idx ] ) )
            return &instr[ idx ];
    } // for, len

    return NULL;
}

// find keyword from the string without being fooled by false find
// Input string must be NULL terminated.
char* strsearch( char* instr, const char* keyword )
{
    char* pch = strstr( instr, keyword );

    if ( ( pch == instr ) ||
        ( isspace( *( pch - 1 ) ) &&
        isspace( *( pch + strlen( keyword ) ) )
        )
        )
        return pch;
    else
        return NULL;
}

// Prints file content in hexadecimal byte values to output file pointer
// Caller must have valid fpout. This function does not close fpout after
// getting called.
void dumpFileContent( IN FILE *fpout, IN const char *file )
{
    u8_t data;
    u32_t count = 0;
    FILE* fp = fopen( file, "rb" );
    char schar[17] = {0};
    if ( fp == NULL )
        return;

    fprintf( fpout, "Dumping file content for \"%s\":\n", file );
    fprintf( fpout, "--------------------------------------\n" );
    fprintf( fpout, "%x: ", count );
    while( fread( &data, sizeof( data ), 1, fp ) == 1 ) {
        fprintf( fpout, "%02x ", data );
        schar[ count % 16 ] = isprint( ( int )data ) ? ( char )data : '.';
        if ( ( ++count % 16 ) == 0 ) {
            fprintf( fpout, ": %s\n", schar );
            memset( schar, 0, sizeof( schar ) );
            fprintf( fpout, "%x: ", count );
        }
    } // while, fread
    fprintf( fpout, ": %s\n", schar );

    fclose( fp );
}

// Prints memorybuffer content in hexadecimal byte values
// only BYTE, WORD, DWORD, QWORD width type supported
void dumpMemoryContent( IN FILE *fpout, IN u8_t *buf, u32_t len,
                        SCN_DATA_TYPE widthType )
{
    word_t word;
    dword_t dword;
    qword_t qword;
    u32_t count = 0;
    char schar[17] = {0};

    fprintf( fpout, "Dumping memory content at %p:\n", buf );
    fprintf( fpout, "--------------------------------------\n" );
    fprintf( fpout, "%p: ", buf + count );
    while( count < len ) {
        if ( widthType == SCN_DATA_WORD ) {
            memcpy( &word, &buf[ count ], 2 );
            fprintf( fpout, "%04x ", word );
            count += 2;
        } else if ( widthType == SCN_DATA_DWORD ) {
            memcpy( &dword, &buf[ count ], 4 );
            fprintf( fpout, "%08x ", dword );
            count += 4;
        } else if ( widthType == SCN_DATA_QWORD ) {
            memcpy( &qword, &buf[ count ], 8 );
            fprintf( fpout, "%016llx ", qword );
            count += 8;
        } else {
            fprintf( fpout, "%02x ", buf[ count ] );
            schar[ count % 16 ] =
                isprint( ( int )buf[ count ] ) ? ( char )buf[ count ] : '.';
            ++count;
        }

        if ( ( count % 16 ) == 0 ) {
            if ( schar[0] )
                fprintf( fpout, ": %s", schar );
            fprintf( fpout, "\n%p: ", buf + count );
        }
    } // while, count < len
    fprintf( fpout, "\n" );
}

char* clearCommand (char* instr)
{
    int len = strlen(instr);
    instr[ len - 1 ] = '\0';
    return instr;
}

int strToArgs (IN char* instr, OUT char argv[][ARG_LEN] )
{
    char *token;
    char *brk;
    int argc = 0;

    token = strtok_r( instr, " -\n", &brk);

    while (token != NULL){
        strcpy( *argv, token );
        ++ argv;
        token = strtok_r ( NULL, " -\n", &brk);
        ++ argc;
    }
    return argc;

}
*/