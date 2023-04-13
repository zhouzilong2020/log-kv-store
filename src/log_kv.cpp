/**
 * This file implements the <code>log_kv</code>
 * class that wraps up the <code>log</code> class.
 * It exposes a hash table like set of APIs to the
 * client.
 */

#include <log.h>
#include <log_kv.h>
#include <utility.h>

LogKV::LogKV()
{
    log = new Log();
    tableSize = 0;
}

int LogKV::put(std::string &key, const std::string *val)
{
    auto it = kvTable.find(key);
    void *logPtr;
    if (it == kvTable.end())
    {
        logPtr = log->append(0, key, val);
        tableSize++;
    }
    else
    {
        Entry *entry = (Entry *)it->second;
        logPtr = log->append(entry->version + 1, key, val);
    }

    kvTable[key] = logPtr;
    return 0;
}

std::unique_ptr<std::string> LogKV::get(const std::string &key)
{
    auto it = kvTable.find(key);
    if (it == kvTable.end())
    {
        return nullptr;
    }

    Entry *entry = (Entry *)it->second;
    using UniqueStrPtr = std::unique_ptr<std::string>;
    UniqueStrPtr ptr =
        UniqueStrPtr(new std::string((char *)&entry->payload + entry->keySize));

    return ptr;
}

void LogKV::deleteK(std::string &key)
{
    auto it = kvTable.find(key);
    if (it == kvTable.end())
    {
        return;
    }

    log->append(-1, key, NULL);
    kvTable.erase(it);
    tableSize--;
    return;
}

size_t LogKV::size()
{
    return tableSize;
}
