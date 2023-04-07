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
}

int LogKV::put(std::string key, std::string val)
{
    auto it = kvTable.find(key);
    void *logPtr;
    if (it == kvTable.end())
    {
        logPtr = log->append(0, key.c_str(), val.c_str());
    }
    else
    {
        Entry *entry = (Entry *)it->second;
        logPtr = log->append(entry->version + 1, key.c_str(), val.c_str());
    }

    kvTable[key] = logPtr;
    return 0;
}

std::string LogKV::get(std::string key)
{
    auto it = kvTable.find(key);
    if (it == kvTable.end())
    {
        return std::string();
    }

    Entry *entry = (Entry *)it->second;
    std::string res((char *)&entry->payload + entry->keySize);

    return res;
}

void LogKV::deleteK(std::string key)
{
    auto it = kvTable.find(key);
    if (it == kvTable.end())
    {
        return;
    }

    log->append(-1, key.c_str(), NULL);
    kvTable.erase(it);
    return;
}