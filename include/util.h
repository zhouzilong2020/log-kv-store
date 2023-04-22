/**
 * This file defines all the helper functions.
 */

#ifndef __UTIL_H__
#define __UTIL_H__
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <string>
#include <vector>

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