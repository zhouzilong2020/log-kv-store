/**
 * This file defines all the helper functions.
 */

#ifndef __UTIL_H__
#define __UTIL_H__
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

/**
 * getOpTime returns the time interval, which is the
 * difference between end and start, in milliseconds
 *
 */
std::string getTimeFormat(const std::string &format = "%Y-%m-%d");

#endif