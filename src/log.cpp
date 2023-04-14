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
    // initialize the meta data
    entryCnt = 0;
    byteSize = 0;
    lastWrite = 0;
    fileCnt = 0;
    currentLogSize = 0;

    // initialize the first memory chunk
    expend();
}

void *Log::append(int version, std::string &key, const std::string *val)
{
    const static int offset = offsetof(Entry, payload);

    int keySize = key.size() + 1;  // including the null char
    int valSize = val == NULL ? 0 : val->size() + 1;
    int entrySize = offset + keySize + valSize;

    // check size
    if (entrySize + currentLogSize > ChunkSize)
    {
        expend();
    }
    currentLogSize += entrySize;

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

    return newEntry;
}

void Log::recover()
{
}
