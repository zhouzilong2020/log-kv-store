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

struct Entry
{
    int version;
    long keySize;
    long valSize;
    void *payload;
};

class Log
{
   public:
    /**
     * This function append the key-value pair,
     * together with the version number of the event
     * into the log.
     *
     * Note that a -1 version number stands for a deletion.
     */
    void append(char *key, char *value, int version);

    /**
     * This function load the log from the disk to recover
     * the in-memory log, after a failure.
     */
    int recover();

   private:
    void *logHead;      ///< points to the start of first log segment
    void *logSegHead;   // points to the first writable byte of the current log
    long size;          // record the number of entries currently in the log
    long byteSize;      // log size in bytes
    int writeInterval;  // specifies the time interval that a disk write will
                        // be triggered

    /*
     * expend expends the log.
     */
    int expend();

    /*
     * compact compacts the log, removing unnecessary entries.
     */
    int compact();

    /*
     * write2Disk writes the current log to the disk.
     */
    int write2Disk();

    /**
     * timerTrigger triggers the disk write periodically, even if the log
     * capacity is not reached
     */
    int timerTrigger();
};

#endif