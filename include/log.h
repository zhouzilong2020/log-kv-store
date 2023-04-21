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
struct PersistentMetaInfoFile
{
    uint64_t createdTs;
    uint64_t updatedTs;
    uint64_t tail;      // tail points to the last log in this file
    uint64_t chunkCnt;  // the number of chunk in the file
    PersistentMetaInfoFile() {}
    PersistentMetaInfoFile(uint64_t tail, uint64_t chunkCnt)
        : tail(tail), chunkCnt(chunkCnt)
    {
        createdTs = getTS();
        updatedTs = createdTs;
    }
};
typedef struct PersistentMetaInfoFile PersistentMetaInfoFile;

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

   private:
    Chunk *head;                     // head points to current chunk
    std::vector<Chunk *> chunkList;  // a list of chunks

    int fileCnt;           // persist log file number
    int nextPersistChunk;  // nextPersistChunk points to the next log
                           // that needed to be persisted to disk
    bool recovering;  // indicate that the logkv is replaying the previous log

    const uint64_t ChunkSize = 1 << 21;  // 2Mb chunk size
    const uint64_t FileSize = 1 << 31;   // 2Gb file size

    /*
     * expend allocates a new chunk of memory,  the log.
     */
    void expend()
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

    /*
     * write2Disk writes the current log to the disk.
     */
    int persist()
    {
        if (recovering) return 0;

        std::string filename = PersistRoot + "/log-" + std::to_string(fileCnt);
        PersistentMetaInfoFile fileMeta;

        // create if not exist, otherwise open
        std::fstream fp;

        fp.open(filename, std::ios::app);
        if (fp.tellg() == 0)  // newly created file
        {
            // init file meta info
            fileMeta =
                PersistentMetaInfoFile(sizeof(PersistentMetaInfoFile), 0);
            fp.write((char *)&fileMeta, sizeof(PersistentMetaInfoFile));
            // printf("Persist file header %llu %llu %llu %llu\n",
            //        fileMeta.createdTs, fileMeta.updatedTs, fileMeta.chunkCnt,
            //        fileMeta.tail);
        }
        fp.close();

        // printf("\nPersisting chunk %llu to %s\n", fileMeta.chunkCnt + 1,
        //        filename.c_str());

        fp.open(filename);
        fp.seekg(0, std::ios::beg);
        fp.read((char *)&fileMeta, sizeof(PersistentMetaInfoFile));

        // write ahead
        fp.seekp(0, std::ios::end);
        fp.write((char *)head, sizeof(Chunk));
        fp.write(head->getHead(), head->getCapacity());
        // printf("Persist chunk info %d %d %d %d %d\n", head->get(CREATEDTS),
        //        head->get(UPDATEDTS), head->get(ENTRYCNT),
        //        head->get(CAPACITY), head->get(USED));

        // Entry *logEntry = (Entry *)head->getHead();
        // printf("Persist first Entry info %d %d %d | %s <-> %s\n",
        //        logEntry->version, logEntry->keySize, logEntry->valSize,
        //        (char *)&logEntry->payload,
        //        (char *)&logEntry->payload + logEntry->keySize);

        fileMeta.updatedTs = getTS();
        fileMeta.chunkCnt++;
        fileMeta.tail += ChunkSize;
        fp.seekp(0, std::ios::beg);
        // commit begins
        fp.write((char *)&fileMeta, sizeof(PersistentMetaInfoFile));
        fp.close();
        // commit ends

        nextPersistChunk++;
        // persist to another file if the current file is full
        if (fileMeta.chunkCnt * ChunkSize == FileSize) fileCnt++;
        return 0;
    }

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