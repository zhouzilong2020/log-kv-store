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

extern std::chrono::time_point<std::chrono::high_resolution_clock> start;

/**
 * loadTrace load the trace generated by interposing
 * wrapper function.
 */
int loadTrace(std::string f_path);

/**
 * myMalloc interposes the c <code>malloc</code>
 * and record the traces for evaluation and analysis.
 */
void *myMalloc(size_t size);

/**
 * simuFail simulates a key-value store machine
 * failure to test the recovery speed and completeness.
 */
void *simuFail();

/**
 * setClock set the start high resolution clock
 */
void *setClock();

/**
 * getOpTime returns the time interval, which is the
 * difference between end and start, in milliseconds
 *
 */
std::chrono::microseconds getOpTime();

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