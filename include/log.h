/**
 * This file defines the <code>my_log</code>
 * class.
 */

/**
 * Each entry in the log will be composed of
 * four attributes.
 */
#ifndef __LOG_H__
#define __LOG_H__
#include <chunk.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "../include/util.h"

// TODO: make it configurable
static std::string PersistRoot = "../.persist";

// on disk meta info, one per persistent file
struct MetaInfoPersistentFile
{
    uint64_t createdTs;
    uint64_t updatedTs;
    uint64_t tail;      // tail points to the last log in this file
    uint64_t chunkCnt;  // the number of chunk in the file
    MetaInfoPersistentFile()
    {
        tail = sizeof(MetaInfoPersistentFile);
        chunkCnt = 0;
        createdTs = getTS();
        updatedTs = createdTs;
    }
    MetaInfoPersistentFile(uint64_t tail, uint64_t chunkCnt)
        : tail(tail), chunkCnt(chunkCnt)
    {
        createdTs = getTS();
        updatedTs = createdTs;
    }
    void addChunk(int chunkSize)
    {
        updatedTs = getTS();
        chunkCnt++;
        tail += chunkSize;
    }
};
typedef struct MetaInfoPersistentFile MetaInfoPersistentFile;

/**
 * The log data structure uses a singly linked
 * sub arrays as the underlying data structure.
 * The sub arrays are of fixed length (2MB), which
 * allocated on the heap when the current space is
 * not enough.
 */
class Log
{
   public:
    Log();
    Log(std::unordered_map<std::string, Entry *> *kvTable);
    
    ~Log();
    
    /**
     * This function append the key-value pair,
     * together with the version number of the event
     * into the log.
     *
     * Note that a -1 version number stands for a deletion.
     * val is nullable so we use pointer here.
     */
    Entry *append(const int version, const std::string &key,
                  const std::string *val);

    /**
     * This function sets the recover indicator
     */
    void recover(bool recover) { recovering = recover; };

    /**
     * getChunkHead returns the chunkList vector
     */
    const std::vector<Chunk *> *getChunkList() { return &chunkList; }

    int chunkSize() { return chunkList.size(); };

    int currentChunkUsed() { return head->getCapacity(); };

    int currentChunkCapacity() { return chunkList.size(); };

    int persist();

    void hideFile();

    void removePersistedFile();

   private:
    Chunk *head;                     // head points to current chunk
    std::vector<Chunk *> chunkList;  // a list of chunks
    std::unordered_map<std::string, MetaInfoPersistentFile *>
        persistentFileTable;  // on disk file, filename->metaInfo

    int currentFileId;     // currentFileId is the id of the current persistent
                           // file
    int nextPersistChunk;  // nextPersistChunk points to the next log
                           // that needed to be persisted to disk
    bool recovering;  // indicate that the logkv is replaying the previous log

    const uint64_t ChunkSize = 1 << 21;  // 2Mb chunk size
    const uint64_t FileSize = 1 << 31;   // 2Gb file size

    /*
     * expend allocates a new chunk of memory,  the log.
     */
    void expend(bool doPersist = true)
    {
        Chunk *newChunk = new Chunk(ChunkSize);
        if (newChunk == NULL)
        {
            printf("Log::expend: OOM failed to allocate new chunk\n");
            exit(1);
        }
        // write back happens whenever a segment is filled
        if (chunkList.size() != 0) persist();

        chunkList.push_back(newChunk);
        head = newChunk;
    }

    /*
     * compact compacts the log, removing unnecessary entries.
     */
    int compact() { return 1; }

    /**
     * timerTrigger triggers the disk write periodically, even if the log
     * capacity is not reached
     */
    int timerTrigger()
    {
        // TODO
        return 1;
    }
};

#endif