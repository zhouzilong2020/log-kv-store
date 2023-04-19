/**
 * This file implements the <code>log_kv</code>
 * class that wraps up the <code>log</code> class.
 * It exposes a hash table like set of APIs to the
 * client.
 */

#include <dirent.h>
#include <log.h>
#include <log_kv.h>
#include <util.h>

LogKV::LogKV()
{
    log = new Log();
    tableSize = 0;
}

int LogKV::put(std::string &key, const std::string *val)
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
}

size_t LogKV::size()
{
    return tableSize;
}

int LogKV::recover()
{
    DIR *dr;
    struct dirent *it;
    std::vector<std::pair<int, std::string>> filenames;

    // gather all files to be read
    dr = opendir(persistRoot.c_str());
    if (dr)
    {
        while ((it = readdir(dr)) != NULL)
        {
            printf("%s\n", it->d_name);
            filenames.push_back(
                std::pair<int, std::string>(it->d_namlen, it->d_name));
        }
        // sort it in log file generation order
        std::sort(filenames.begin(), filenames.end());
    }

    std::fstream fp;
    PersistentMetaInfoFile fileMeta;
    char *logBuf =
        (char *)malloc(RecoverBufSize);  // 512Mb buffer for log replay

    Chunk *chunk;
    char *chunkPayload;
    uint64_t chunkOffset;

    // read through all files
    for (auto &i : filenames)
    {
        printf("Start: %s\n", i.second.c_str());
        fp.open(i.second, std::ios::binary | std::ios::in);
        fp.seekg(0, std::ios::beg);
        // read te file meta
        fp.read((char *)&fileMeta, sizeof(PersistentMetaInfoFile));

        while (!fp.eof())
        {
            // read the 512Mb log to parse each I/O
            fp.read((char *)logBuf, RecoverBufSize);

            // replay the log in terms of chunk objects
            chunkOffset = 0;
            while (chunkOffset < RecoverBufSize)
            {
                // get chunk meta and payload
                chunk = (Chunk *)(logBuf + chunkOffset);
                chunkPayload = logBuf + chunkOffset + sizeof(Chunk);

                // replay the entries in the current chunk
                replayChunk(chunk, chunkPayload);

                // go to the next chunk in this 512Mb log slice
                chunkOffset += sizeof(Chunk) + chunk->getCapacity();
            }
        }
    }

    return 0;
}
