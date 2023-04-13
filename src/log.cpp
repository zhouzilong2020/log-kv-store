/**
 * This file implements the <code>my_log</code>
 * class, which supports the construction and
 * maintenance of the log data structure.
 */

#include <log.h>
#include <utility.h>

#include <string>

Log::Log()
{
    // initialize the meta data
    entryCnt = 0;
    byteSize = 0;
    lastWrite = 0;
    fileCnt = 0;

    // initialize the first memory chunk
    head = std::malloc(ChunkSize);
    logList.push_back(head);
}

void *Log::append(int version, std::string &key, const std::string *val)
{
    int keySize = key.size() + 1;  // including the null char
    int valSize = val == NULL ? 0 : val->size() + 1;
    int entrySize = 6 + keySize + valSize;

    // check size
    if (entrySize > currentLogSize)
    {
        expend();
    }

    Entry *newEntry = (Entry *)head;
    newEntry->version = version;
    newEntry->keySize = keySize;
    newEntry->valSize = valSize;
    // FIXME: is memory alignment a problem?
    strlcpy((char *)&newEntry->payload, key.c_str(), newEntry->keySize);
    if (val)
        strlcpy((char *)&newEntry->payload + keySize, val->c_str(),
                newEntry->valSize);
    head = (char *)head + entrySize;

    entryCnt++;
    byteSize = byteSize + entrySize;

    return newEntry;
}

void Log::recover()
{
}
