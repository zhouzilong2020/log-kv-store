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
static std::string persistRoot = "./.persist";

// on disk meta info, one per persistent file
struct PersistentMetaInfoFile
{
    uint64_t createdTs;
    uint64_t updatedTs;
    uint64_t tail;      // tail points to the last log in this file
    uint64_t chunkCnt;  // the number of chunk in the file
    PersistentMetaInfoFile() {}
    PersistentMetaInfoFile(uint64_t ptail, uint64_t chunkCnt)
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
    void recover(std::unordered_map<std::string, Entry *> &kvTable) { return; };

   private:
    Chunk *head;                     // head points to current chunk
    std::vector<Chunk *> chunkList;  // a list of chunks

    int fileCnt;           // persist log file number
    int nextPersistChunk;  // nextPersistChunk points to the next log
                           // that needed to be persisted to disk

    const uint64_t ChunkSize = 1 << 21;               // 2Mb chunk size
    const uint64_t FileSize = 1 << 31;                // 2Gb file size

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
        if (chunkList.size() != 1) persist();
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
        std::string filename = persistRoot + "/log-" + std::to_string(fileCnt);
        PersistentMetaInfoFile fileMeta;

        // create if not exist, otherwise open
        std::fstream fp(filename, std::ios::binary | std::ios::in |
                                      std::ios::out | std::ios::ate);
        std::cout << "file size " << fp.tellp() << std::endl;
        if (fp.tellg() == 0)  // newly created file
        {
            // init file meta info
            printf("init file meta info\n");
            fileMeta =
                PersistentMetaInfoFile(sizeof(PersistentMetaInfoFile), 0);
            fp.write((char *)&fileMeta, sizeof(PersistentMetaInfoFile));
        }
        else  // file already exists
        {
            fp.seekg(0, std::ios::beg);
            fp.read((char *)&fileMeta, sizeof(PersistentMetaInfoFile));
        }

        printf("file status: %llu %llu %llu %llu\n", fileMeta.chunkCnt,
               fileMeta.createdTs, fileMeta.updatedTs, fileMeta.tail);

        // write ahead
        fp.write((char *)head, sizeof(Chunk));
        fp.write(head->getHead(), head->getCapacity());

        fileMeta.updatedTs = getTS();
        fileMeta.chunkCnt++;
        fileMeta.tail += ChunkSize;
        fp.seekp(0, std::ios::beg);
        // commit begins
        fp.write((char *)&fileMeta, sizeof(PersistentMetaInfoFile));
        fp.close();
        // commit ends

        nextPersistChunk++;
        // ready to write next file when the current file reachs 2Gb
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