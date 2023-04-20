/**
 * This file implements the <code>log_kv</code>
 * class that wraps up the <code>log</code> class.
 * It exposes a hash table like set of APIs to the
 * client.
 */

#include <log.h>
#include <log_kv.h>
#include <util.h>

LogKV::LogKV()
{
    log = new Log();
    tableSize = 0;
}

int LogKV::put(const std::string &key, const std::string *val)
{
    auto it = kvTable.find(key);
    Entry *newEntry;
    if (it == kvTable.end())
    {
        newEntry = log->append(0, key, val);
        tableSize++;
    }
    else
    {
        Entry *oldEntry = it->second;
        if (oldEntry == NULL)
        {
            return -1;
        }
        newEntry = log->append(oldEntry->version + 1, key, val);
    }

    kvTable[key] = newEntry;
    return 0;
}

std::unique_ptr<std::string> LogKV::get(const std::string &key)
{
    auto it = kvTable.find(key);
    if (it == kvTable.end())
    {
        return nullptr;
    }

    Entry *entry = it->second;
    using UniqueStrPtr = std::unique_ptr<std::string>;
    return UniqueStrPtr(
        new std::string((char *)&entry->payload + entry->keySize));
}

void LogKV::deleteK(const std::string &key)
{
    auto it = kvTable.find(key);
    if (it == kvTable.end())
    {
        return;
    }

    log->append(-1, key, NULL);
    kvTable.erase(it);
    tableSize--;
}

size_t LogKV::size()
{
    return tableSize;
}
