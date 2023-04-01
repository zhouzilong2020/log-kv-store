/**
 * This file implements the <code>my_log</code>
 * class, which supports the construction and
 * maintenance of the log data structure. 
*/

#include <log.h>
#include <util.h>

/**
 * The log data structure uses a singly linked 
 * subarrays as the underlying data structure.
 * The subarrays are of fixed length (2MB), which
 * allocated on the heap when the current space is 
 * not enough.
*/



