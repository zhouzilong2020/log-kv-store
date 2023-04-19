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
#include <util.h>

#include <cstdint>
#include <vector>

struct Entry
{
    uint16_t version;
    uint16_t keySize;
    uint16_t valSize;
    void *payload;
};

// on disk meta info, one per persistent file
struct PersistentMetaInfoFile
{
    uint64_t createdTs;
    uint64_t updatedTs;
    uint64_t tail;    // tail points to the last log in this file
    uint64_t logCnt;  // the number of logs in the file
};
typedef struct PersistentMetaInfoFile PersistentMetaInfoFile;

// on disk meta info, one per log
struct PersistentMetaInfoLog
{
    uint64_t createdTs;
    uint64_t updatedTs;
    uint64_t size;      // size is the size of the log
    uint64_t entryCnt;  // the number of log entry that stores in this log
    PersistentMetaInfoLog(){};  // default constructor
    PersistentMetaInfoLog(uint64_t &size, uint64_t &entryCnt)
    {
        uint64_t now = getTS();
        createdTs = now;
        updatedTs = now;
        size = size;
        entryCnt = entryCnt;
    };
};
typedef struct PersistentMetaInfoFile PersistentMetaInfoFile;

/**
 * The log data structure uses a singly linked
 * subarrays as the underlying data structure.
 * The subarrays are of fixed length (2MB), which
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
    void *append(int version, std::string &key, const std::string *val);

    /**
     * This function load the log from the disk to recover
     * the in-memory log, after a failure.
     */
    void recover();

   private:
    void *head;  // points to the start of first log segment
    int currentLogSize;

    // points to the first writable byte of the current log
    std::vector<void *> logList;

    long entryCnt;  // record the number of entries currently in the log
    long byteSize;  // log size in bytes
    int lastWrite;  // specifies the time interval that a disk write will
                    // be triggered
    int fileCnt;

    const int ChunkSize = 1 << 21;    // 2Mb chunk size
    const int WriteBackInterval = 5;  // 5s

    /*
     * expend allocates a new chunk of memory,  the log.
     */
    void expend()
    {
        printf("expend the log space\n");
        // expend the log space by default chunk
        void *new_head = std::malloc(ChunkSize);
        logList.push_back(new_head);
        head = new_head;
        currentLogSize = 0;

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