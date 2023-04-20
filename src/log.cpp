/**
 * This file implements the <code>my_log</code>
 * class, which supports the construction and
 * maintenance of the log data structure.
 */

#include <log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <util.h>

#include <filesystem>
#include <fstream>
#include <string>

Log::Log(std::unordered_map<std::string, Entry *> *kvTable)
{
    static int cnt = 0;
    cnt++;

    head = NULL;
    fileCnt = 0;
    nextPersistChunk = 0;
    expend(false);
    for (auto &kv : *kvTable)
    {
        auto newEntry = head->append(kv.second);
        if (!newEntry)
        {
            expend(false);
            newEntry = head->append(kv.second);
        }
        kv.second = newEntry;
    }
}

Log::Log()
{
    head = NULL;
    fileCnt = 0;
    nextPersistChunk = 0;
    expend();

    // initialize persist config
    if (stat(persistRoot.c_str(), NULL) == -1)
    {
        // owner can read/write, group can read
        mkdir(persistRoot.c_str(), 0777);
    }
}

Entry *Log::append(const int version, const std::string &key,
                   const std::string *val)
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
