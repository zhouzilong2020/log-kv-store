/**
 * This file defines all the helper functions.
 */

#ifndef __UTIL_H__
#define __UTIL_H__
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <string>
#include <vector>

struct recoveryExp
{
    uint64_t cmdNum;
    uint naivePersistByte;
    uint64_t logKV_duration;
    uint64_t naiveKV_duration;
    uint64_t logKV_fail;
    uint64_t naiveKV_fail;
    uint64_t logKV_miss;
    uint64_t naiveKV_miss;
    uint64_t logKVSize;
    uint64_t naiveSize;
    uint64_t gtSize;
};

uint64_t getTS();

/**
 * listDir lists all files within the given directory path
 */
void listDir(const char *path, std::vector<std::string> &files);

/**
 * removeDir remove the given directory
 */
void removeDir(const char *path);

/**
 * existDir returns try if the given directory exists
 */
bool existDir(const char *path);

#endif