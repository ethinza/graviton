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

#include "fileutility.h"

#include <dirent.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <fstream>

static bool IsFileInFileList(const char* fileList, const char* filename)
{
    if (filename == nullptr)
    {
        return false;
    }

    if (fileList != nullptr)
    {
        const char* entry = fileList;

        while (*entry)
        {
            if (strcmp(filename, entry) == 0)
            {
                return true; // match found
            }

            // seek to start of next string
            entry += (strlen(entry) + 1);
        }
    }

    return false;        // no match found
}


static bool IsFolderInFileList(const char* fileList, const char* folder)
{
    if (folder != nullptr && fileList != nullptr)
    {
        const char* entry = fileList;

        while (*entry)
        {
            // make copy of filename, then truncate filename part
            char tempFolder[256];
            strcpy(tempFolder, entry);

            while (strlen(tempFolder) > 1)
            {
                if (strcmp(folder, tempFolder) == 0)
                {
                    return true; // match found
                }

                FileUtility::StripFilenameFromPath(tempFolder);
            }

            entry += (strlen(entry) + 1);  // seek to start of next stringclTabCtrl
        }
    }

    return false;                     // no match found
}

void FileUtility::StripFilenameFromPath(char* path)
{
    if (path != nullptr)
    {
        int index = strlen(path);
        
        if (index >= 2)
        {
            while (--index >= 0)
            {
                if ((path[index] == '/') || (path[index] == '\\'))
                {
                    if (index == 0)
                    {
                        path[1] = '\0';          // special case for root folder
                    }
                    else
                    {
                        path[index] = '\0';     // truncate pathname
                    }

                    break;
                }
            }
        }
    }
}

bool FileUtility::SafeDelete(const char* filename)
{
    bool ok = false;

    if (filename == nullptr)
    {
        printf("Error deleting file '%s'.  Invalid argument.", "<null>");
    }
    else if (FileExists(filename))
    {
        if (SetAttribute(filename, 777))
        {
            ok = remove(filename) == 0;
        }
    }

    return ok;
}

bool FileUtility::Remove(const char* directory)
{
    return rmdir(directory);
}

bool FileUtility::Create(const char* directory, mode_t mode)
{    
    return mkdir(directory, mode) == 0;
}

bool FileUtility::CreateIfDoesNotExist(const char* directory, mode_t mode)
{
    bool ok = FolderExists(directory);

    if (!ok)
    {
        ok = FileUtility::Create(directory, mode);
    }

    return ok;
}

bool FileUtility::CreatePath(const char* folder, mode_t mode)
{
    bool ok = true;

    if (folder == nullptr || ((folder[0] != '/') && (folder[0] != '\\')))
    {
        ok = false;
    }
    else
    {
        // make temp copy of fully-qualified path
        char workingPath[256];
        const char* currentPath = folder;
        strcpy(workingPath, folder);

        // loop until we fall off the end of the string

        while (ok && *currentPath)
        {
            currentPath++;                              // skip over leading '/' or '\\'

            // advance to end of next folder
            while (*currentPath && (*currentPath != '/') && (*currentPath != '\\'))
            {
                currentPath++;
            }
           
            workingPath[currentPath - folder] = '\0';   // temporarily truncate working path

            if (!FolderExists(workingPath))                   // create directory, if it doesn't exist
            {
                ok = Create(workingPath, mode);
            }
            
            workingPath[currentPath - folder] = *currentPath; // restore truncated working path
        }
    }

    return ok;
}


String FileUtility::MakePath(const char* directory, const char* filename)
{
    String path(directory);

    if (path.length() > 0 && strlen(filename))
    {
        if ((path[path.length() - 1] != '/') &&
            (path[path.length() - 1] != '\\') &&
            (filename[0] != '/') &&
            (filename[0] != '\\'))
        {
            path.append("\\"); // add path separator
        }
    }

    path.append(filename);
    return path;
}

bool FileUtility::FileExists(const char* filename)
{
    bool ok = false;

    if (filename != nullptr)
    {
        struct stat buffer;
        ok = (stat(filename, &buffer) == 0);

        // ok = access(filename, 0) == 0;
    }

    return ok;
}

bool FileUtility::FolderExists(const char* folder)
{
    bool ok = false;

    if (folder != nullptr)
    {
        struct stat info;
        ok = stat(folder, &info) == 0 && (info.st_mode & S_IFDIR);
    }

    return ok;
}

bool FileUtility::SetAttribute(const char* filename, mode_t mode)
{
    return chmod(filename, mode) == 0;
}

bool FileUtility::DeleteEntry(const char* filename, bool isDirectory, DeleteFileCallbackType callback)
{
    bool ok = SetAttribute(filename, 777);     // make read-only folders writable

    if (ok)
    {
        if (callback != nullptr)
        {
            callback(filename);
        }

        if (isDirectory)
        {
            ok = Remove(filename);
        }
        else
        {
            ok = SafeDelete(filename);
        }
    }

    return ok;
}

//todo
//bool FileUtility::DeleteContents(const char* folder, const char* keepFiles, DeleteFileCallbackType callback)
//{
//    bool ok = true;
//
//    printf("Deleting folder '%s' contents.", folder);
//
//    if (!Exists(folder))
//    {
//        printf("Folder '%s' not found.", folder);
//        ok = false;
//    }
//    else
//    {
//        WIN32_FIND_DATA findData;
//        string filename;
//        string dirSpec = MakePath(folder, "*");     // wildcard for all directory contents
//        HANDLE hFind = FindFirstFile(dirSpec, &findData);
//
//        if (hFind != INVALID_HANDLE_VALUE)
//        {
//            do
//            {
//                filename = MakePath(folder, findData.cFileName);                // form full pathname to folder item
//                bool isDirectory = findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
//
//                if (isDirectory)
//                {
//                    if (IsFolderInFileList(keepFiles, filename.c_str()))        // delete folder, unless it is a keeper
//                    {
//                        printf("Skipping directory '%s'.  Excluded.", filename.c_str());
//                    }
//                    else
//                    {
//                        ok = DeleteContents(filename.c_str(), keepFiles, callback);     // recursively delete sub-directory
//
//                        if (ok)
//                        {
//                            ok = DeleteEntry(filename.c_str(), isDirectory, callback);
//                        }
//                    }
//                }
//                else
//                {
//                    if (IsFileInFileList(keepFiles, filename.c_str()))          // delete a file, unless it is a keeper
//                    {
//                        printf("Skipping file '%s'.  Excluded.", filename.c_str());
//                    }
//                    else
//                    {
//                        DeleteEntry(filename.c_str(), isDirectory, callback);
//                    }
//                }
//            } while (ok && FindNextFile(hFind, &findData));
//
//            FindClose(hFind);
//        }
//    }
//
//    return ok;
//}

bool FileUtility::HasExtension(const char* filename, const char* extension)
{
    int len = strlen(filename);
    int found = false;

    if (len > 1)
    {
        // Search backwards from the end of the string to the last '.' character.

        const char* lastDot = filename + len;

        while (--lastDot >= filename && !found)
        {
            if (*lastDot == '.')
            {
                // Found start of file extension, so check to see if it matches one of the known extensions for executable files.

                if (strcasecmp(lastDot, extension) == 0)
                {
                    found = true;
                }
            }
        }
    }

    return found; 
}

bool FileUtility::GetFileCount(const char* directory, int& fileCount, int& folderCount, bool includeSubdirectories)
{
    bool ok = true;

    if (!includeSubdirectories)
    {
        fileCount = 0;
        folderCount = 0;
    }

    if (!FolderExists(directory))
    {
        ok = false;
    }
    else
    {
        struct dirent* dirInfo;
        DIR* dir = opendir(directory);

        if (dir != nullptr)
        {
            ok = true;

            while ((dirInfo = readdir(dir)) != nullptr)
            {
                struct stat fileInfo;

                if (stat(dirInfo->d_name, &fileInfo) == 0 && (fileInfo.st_mode & S_IFDIR))
                {
                    folderCount++;

                    if (includeSubdirectories)
                    {
                        String subdir = FileUtility::MakePath(directory, dirInfo->d_name);
                        ok = GetFileCount(subdir.c_str(), fileCount, folderCount, includeSubdirectories);
                    }
                }
                else
                {
                    fileCount++;
                }
            }

            closedir(dir);
        }
        else
        {
            ok = false;
        }
    }

    return ok;
}

bool FileUtility::RenameFile(const char* fromFilename, const char* toFilename)
{
    return rename(fromFilename, toFilename) == 0;
}

u64_t FileUtility::GetFileSize(const char* filename)
{
    struct stat info = {0};
    int rc = stat(filename, &info);

    return rc != -1 ? info.st_size : -1;
}

void FileUtility::GetFolderSize(const char* folder, u64_t& total, u64_t& free)
{
    struct statvfs fsinfo;
    statvfs(folder, &fsinfo);
    total = fsinfo.f_frsize * fsinfo.f_blocks;
    free = fsinfo.f_bsize * fsinfo.f_bfree;  
}

void FileUtility::WriteToFile(const char* fileName, const String& content, bool append)
{
    std::ofstream outf(fileName, std::ofstream::out | (append ? std::ofstream::app : std::ofstream::trunc));

    // read content of file first
    if (outf.is_open())
    {
        outf.write(content.c_str(), content.size());
        outf.close();
    }
}