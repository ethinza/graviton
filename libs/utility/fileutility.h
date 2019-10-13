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

#include <sys/stat.h>

typedef void (*DeleteFileCallbackType) (const char* filename);

class FileUtility
{
private:

    FileUtility() {}
    ~FileUtility() {}
    FileUtility(const FileUtility& rhs) = delete;
    FileUtility& operator=(const FileUtility& rhs) = delete;

    static bool DeleteEntry(const char* filename, bool isDirectory, DeleteFileCallbackType callback);

public:

    static void StripFilenameFromPath(char* path);
    static bool Remove(const char* directory);
    static bool SafeDelete(const char* filename);
    static String MakePath(const char* directory, const char* filename);
    static bool FileExists(const char* folder);
    static bool FolderExists(const char* folder);
    static bool SetAttribute(const char* filename, mode_t mode);
    //todo
    //static bool DeleteContents(const char* folder, const char* keepFiles = nullptr, DeleteFileCallbackType callback = nullptr);
    static bool Create(const char* folder, mode_t mode = ACCESSPERMS);
    static bool CreateIfDoesNotExist(const char* directory, mode_t mode = ACCESSPERMS);
    static bool CreatePath(const char* folder, mode_t mode = ACCESSPERMS);
    static bool HasExtension(const char* filename, const char* extension);
    static bool GetFileCount(const char* directory, int& fileCount, int& folderCount, bool includeSubdirectories = false);
    static bool RenameFile(const char* fromFilename, const char* toFilename);
    static u64_t GetFileSize(const char* filename);
    static void GetFolderSize(const char* folder, u64_t& total, u64_t& free);

    static void WriteToFile(const char* fileName, const String& content, bool append = false);
};

