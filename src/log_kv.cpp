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
    duplicatedEntryCnt = 0;
}

LogKV::~LogKV()
{
    if (log) delete log;
}

void LogKV::put(const std::string &key, const std::string *val)
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
        duplicatedEntryCnt++;
        Entry *oldEntry = it->second;
        if (oldEntry == NULL)
        {
            return;
        }
        newEntry = log->append(oldEntry->version + 1, key, val);
    }

    kvTable[key] = newEntry;

    tryCompact();
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

    duplicatedEntryCnt++;
    log->append(std::numeric_limits<uint16_t>::max(), key, NULL);
    kvTable.erase(it);
    tableSize--;

    tryCompact();
}

size_t LogKV::size()
{
    return tableSize;
}

void LogKV::recover()
{
    log->setRecover(true);

    std::vector<std::string> files;
    listDir(PersistRoot.c_str(), files);

    std::fstream fp;
    MetaInfoPersistentFile fileMeta;
    char *logBuf =
        (char *)malloc(RecoverBufSize);  // 512Mb buffer for log replay

    Chunk *chunk;
    char *chunkPayload;
    uint64_t chunkOffset;
    uint64_t chunkCnt;

    // read through all files
    for (auto &path : files)
    {
        fp.open(path, std::ios::binary | std::ios::in);
        fp.seekg(0, std::ios::beg);
        // read te file meta
        fp.read((char *)&fileMeta, sizeof(MetaInfoPersistentFile));
        while (!fp.eof())
        {
            // read the 512Mb log to parse each I/O
            fp.read((char *)logBuf, RecoverBufSize);
            // replay the log in terms of chunk objects
            chunkOffset = 0;
            chunkCnt = 0;
            while (chunkOffset < RecoverBufSize && chunkCnt < fileMeta.chunkCnt)
            {
                // get chunk meta and payload
                chunk = (Chunk *)(logBuf + chunkOffset);
                chunkPayload = logBuf + chunkOffset + sizeof(Chunk);
                // replay the entries in the current chunk
                replayChunk(chunk, chunkPayload);
                // go to the next chunk in this 512Mb log slice
                chunkOffset += sizeof(Chunk) + chunk->getCapacity();
                chunkCnt++;
            }
        }
        fp.close();
    }
    free(logBuf);
    log->setRecover(false);
}

void LogKV::persist()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    this->log->persist();
    auto end_time = std::chrono::high_resolution_clock::now();
    persistTime++;
    persistDuration += std::chrono::duration_cast<std::chrono::milliseconds>(
                           end_time - start_time)
                           .count();
}