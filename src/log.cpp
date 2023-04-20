/**
 * This file implements the <code>my_log</code>
 * class, which supports the construction and
 * maintenance of the log data structure.
 */

#include <log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <util.h>

#include <fstream>
#include <string>
#include <vector>

Log::Log()
{
    head = NULL;
    fileCnt = 0;
    nextPersistChunk = 0;
    expend();

    // initialize persist directory
    if (!existDir(PersistRoot.c_str()))
    {
        // owner can read/write, group can read
        mkdir(PersistRoot.c_str(), 0777);
    }

    // initialize non-recovering log
    recovering = false;
}
Log::~Log()
{
    for (auto i : chunkList)
    {
        delete i;
    }
}

Entry *Log::append(uint16_t version, std::string &key, const std::string *val)
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
