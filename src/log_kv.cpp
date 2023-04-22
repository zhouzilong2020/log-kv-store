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
    delete log;
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
        duplicatedEntryCnt++;
        Entry *oldEntry = it->second;
        if (oldEntry == NULL)
        {
            return -1;
        }
        newEntry = log->append(oldEntry->version + 1, key, val);
    }

    kvTable[key] = newEntry;

    tryCompact();
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

int LogKV::recover()
{
    log->recover(true);

    std::vector<std::string> files;
    listDir(PersistRoot.c_str(), files);

    std::fstream fp;
    PersistentMetaInfoFile fileMeta;
    char *logBuf =
        (char *)malloc(RecoverBufSize);  // 512Mb buffer for log replay

    Chunk *chunk;
    char *chunkPayload;
    uint64_t chunkOffset;
    uint64_t chunkCnt;

    // read through all files
    for (auto &path : files)
    {
        // printf("\nStart Recovery: %llu %s\n", RecoverBufSize, path.c_str());
        fp.open(path, std::ios::binary | std::ios::in);
        fp.seekg(0, std::ios::beg);
        // read te file meta
        fp.read((char *)&fileMeta, sizeof(PersistentMetaInfoFile));
        // printf("Recovery file header %llu %llu %llu %llu\n",
        // fileMeta.createdTs,
        //        fileMeta.updatedTs, fileMeta.chunkCnt, fileMeta.tail);

        while (!fp.eof())
        {
            // read the 512Mb log to parse each I/O
            fp.read((char *)logBuf, RecoverBufSize);

            // replay the log in terms of chunk objects
            chunkOffset = 0;
            chunkCnt = 0;
            while (chunkOffset < RecoverBufSize && chunkCnt < fileMeta.chunkCnt)
            {
                // printf("\nchunkOffset %llu chunkCnt %llu\n", chunkOffset,
                //        chunkCnt);

                // get chunk meta and payload
                chunk = (Chunk *)(logBuf + chunkOffset);
                chunkPayload = logBuf + chunkOffset + sizeof(Chunk);
                // if (chunkCnt == 0 || chunkCnt == 1 || chunkCnt == 2)
                //     printf(
                //         "Recovery chunk info %d %d %d %d %d | payload offset
                //         "
                //         "%llu\n",
                //         chunk->get(CREATEDTS), chunk->get(UPDATEDTS),
                //         chunk->get(ENTRYCNT), chunk->get(CAPACITY),
                //         chunk->get(USED), chunkOffset + sizeof(Chunk));

                // replay the entries in the current chunk
                replayChunk(chunk, chunkPayload);

                // go to the next chunk in this 512Mb log slice
                chunkOffset += sizeof(Chunk) + chunk->getCapacity();
                chunkCnt++;
            }
        }
    }

    log->recover(false);
    return 0;
}

void LogKV::failure()
{
    kvTable.clear();
    delete log;
    log = new Log;

    // clear out the table size
    tableSize = 0;
}
