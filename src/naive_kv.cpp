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
    if (byteSize > persistByte)
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
    if (byteSize > persistByte)
    {
        persist();
        byteSize = 0;
    }
}

void NaiveKV::persist()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    std::ofstream outFile(persistFile, std::ios::trunc);
    if (outFile.is_open())
    {
        for (const auto& pair : kvTable)
        {
            outFile << pair.first << " " << pair.second << std::endl;
        }
        outFile.close();

        auto end_time = std::chrono::high_resolution_clock::now();
        // calculate the duration
        persistTime++;
        persistDuration +=
            std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                  start_time)
                .count();
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