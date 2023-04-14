/**
 * This file implements the naive key-value
 * store that is based on c++ std::unordered_map
 * and disk snapshot.
 */

#include <naive_kv.h>
#include <util.h>

int NaiveKV::put(const std::string key, const std::string val)
{
    kvTable[key] = val;
    return 0;
}

std::string NaiveKV::get(const std::string key)
{
    auto it = kvTable.find(key);
    if (it == kvTable.end())
    {
        return NULL;
    }

    return it->second;
}

int NaiveKV::deleteK(std::string key)
{
    return kvTable.erase(key);
}