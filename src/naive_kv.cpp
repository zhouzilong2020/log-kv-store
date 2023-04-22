/**
 * This file implements the naive key-value
 * store that is based on c++ std::unordered_map
 * and disk snapshot.
 */

#include <naive_kv.h>
#include <util.h>

#include <fstream>

void NaiveKV::put(const std::string& key, const std::string* val)
{
    kvTable[key] = *val;
    byteSize += key.size() + val->size();
    if (byteSize > 2 << 20)
    {
        persist();
        byteSize = 0;
    }
}

std::unique_ptr<std::string> NaiveKV::get(const std::string& key)
{
    auto it = kvTable.find(key);
    if (it == kvTable.end())
    {
        return NULL;
    }

    using UniqueStrPtr = std::unique_ptr<std::string>;
    return UniqueStrPtr(new std::string(it->second));
}

void NaiveKV::deleteK(const std::string& key)
{
    kvTable.erase(key);
    byteSize += key.size();
    if (byteSize > 2 << 20)
    {
        persist();
        byteSize = 0;
    }
}

void NaiveKV::persist()
{
    std::ofstream outFile(persistFile, std::ios::trunc);
    if (outFile.is_open())
    {
        for (const auto& pair : kvTable)
        {
            outFile << pair.first << " " << pair.second << std::endl;
        }
        outFile.close();
    }
}

void NaiveKV::recover()
{
    std::ifstream inFile(persistFile);
    if (inFile.is_open())
    {
        std::string key, value;
        while (inFile >> key >> value)
        {
            kvTable.emplace(key, value);
        }
        inFile.close();
    }
}
