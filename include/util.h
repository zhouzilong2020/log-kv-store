/**
 * This file defines all the helper functions.
*/

#include <string.h>

/**
 * This function load the trace generated by interposing
 * wrapper function.
*/
int load_trace(std::string f_path);

/**
 * This function interposes the c <code>malloc</code>
 * and record the traces for evaluation and analysis.
*/
void *my_malloc(size_t size);

/**
 * This function simulates a key-value store machine 
 * failure to test the recovery speed and completeness.
*/
void *simu_fail();


