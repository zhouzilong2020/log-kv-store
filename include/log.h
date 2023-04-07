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
#include <cstdint>
#include <vector>

struct Entry
{
    uint16_t version;
    uint16_t keySize;
    uint16_t valSize;
    void *payload;
};

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
     */
    void *append(int version, const char *key, const char *val);

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
        // expend the log space by default chunck
        void *new_head = std::malloc(ChunkSize);
        logList.push_back(new_head);
        head = new_head;

        // write back happens whenever a segment is filled
        write2Disk();
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
    int write2Disk()
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