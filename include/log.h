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
#include <util.h>

#include <cstdint>
#include <vector>

// on disk meta info, one per persistent file
struct PersistentMetaInfoFile
{
    uint64_t createdTs;
    uint64_t updatedTs;
    uint64_t tail;    // tail points to the last log in this file
    uint64_t logCnt;  // the number of logs in the file
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
    /**
     * initLog initialize the log data structure
     */
    Log();

    /**
     * This function append the key-value pair,
     * together with the version number of the event
     * into the log.
     *
     * Note that a -1 version number stands for a deletion.
     * val is nullable so we use pointer here.
     */
    Entry *append(int version, std::string &key, const std::string *val);

    /**
     * This function load the log from the disk to recover
     * the in-memory log, after a failure.
     */
    void recover();

   private:
    const int ChunkSize = 1 << 21;   // 2Mb chunk size
    Chunk *head;                     // head points to current chunk
    std::vector<Chunk *> chunkList;  // a list of chunks

    // int lastWrite;  // specifies the time interval that a disk write will
    //                 // be triggered
    // int fileCnt;
    // const int WriteBackInterval = 5;  // 5s

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
        chunkList.push_back(newChunk);
        head = newChunk;

        // write back happens whenever a segment is filled
        persist();
    }

    /*
     * compact compacts the log, removing unnecessary entries.
     */
    int compact()
    {
        // TODO
        return 1;
    }

    /*
     * write2Disk writes the current log to the disk.
     */
    int persist()
    {
        // TODO
        return 1;
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