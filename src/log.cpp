/**
 * This file implements the <code>my_log</code>
 * class, which supports the construction and
 * maintenance of the log data structure.
 */

#include <log.h>
#include <util.h>

#include <string>

Log::Log()
{
    head = NULL;
    expend();
}

Entry *Log::append(int version, std::string &key, const std::string *val)
{
    Entry *entryPtr = head->append(version, key, val);
    if (entryPtr == NULL)  // the current chunk is full
    {
        expend();
        // this time it should work
        entryPtr = head->append(version, key, val);
    }

    return entryPtr;
}

void Log::recover()
{
}
