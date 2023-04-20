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
    char hash[64];
    uint64_t tail;      // tail points to the last log in this file
    uint64_t chunkCnt;  // the number of chunk in the file
    uint64_t createdTs;
    uint64_t updatedTs;
    PersistentMetaInfoFile()
    {
        chunkCnt = 0;
        tail = sizeof(struct PersistentMetaInfoFile);
        createdTs = getTS();
        updatedTs = createdTs;
    }
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

    ~Log()
    {
        // FIXME:partial persist
        // persist();
        for (auto chunk : chunkList)
        {
            delete chunk;
        }
    }

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
     * This function load the log from the disk to recover
     * the in-memory log, after a failure.
     */
    void recover(std::unordered_map<std::string, Entry *> &kvTable) { return; };

   private:
    Chunk *head;                     // head points to current chunk
    std::vector<Chunk *> chunkList;  // a list of chunks
    std::vector<PersistentMetaInfoFile *> fileList;  // on disk file meta info,
                                                     // one per persistent file

    int fileCnt;              // persist log file number
    u_long nextPersistChunk;  // nextPersistChunk points to the next log
                              // that needed to be persisted to disk

    const uint64_t ChunkSize = 1 << 22;               // 4Mb chunk size
    const uint64_t FileSize = 1 << 31;                // 2Gb file size
    const uint64_t RecoverBufSize = 512 * (1 << 10);  // 512Mb recover buf

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
        chunkList.push_back(newChunk);
        head = newChunk;

        // write back happens whenever a segment is filled
        if (chunkList.size() != 1 && doPersist) persist();
    }

    /*
     * compact compacts the log, removing unnecessary entries.
     */
    int compact() { return 1; }

    /*
     * persist writes the current log to the disk.
     */
    int persist()
    {
        std::string filename = persistRoot + "/log-" + std::to_string(fileCnt);
        PersistentMetaInfoFile *fileMeta = NULL;

        // create if not exist, otherwise open
        std::fstream fp;
        fp.open(filename, std::ios::app);
        std::cout << "open file " << fp.tellg() << std::endl;
        if (fp.tellg() == 0)  // newly created file
        {
            // init file meta info
            printf("init file!!!");
            fileMeta = new PersistentMetaInfoFile();
            fp.write((char *)fileMeta, sizeof(PersistentMetaInfoFile));
            fileList.push_back(fileMeta);
        }
        fp.close();

        fileMeta = fileList[fileList.size() - 1];
        fp.open(filename);
        int addedChunk = 0;
        // do not persist the current working chunk to disk
        for (; nextPersistChunk < chunkList.size() - 1; nextPersistChunk++)
        {
            addedChunk++;
            fp.write((char *)head, sizeof(Chunk));
            fp.write(head->getHead(), head->getCapacity());
        }

        // batch commit begins
        fp.seekp(0, std::ios::beg);
        fileMeta->addChunk(ChunkSize);
        fp.write((char *)fileMeta, sizeof(PersistentMetaInfoFile));
        fp.close();
        // batch commit ends

        // persist to another file if the current file is full
        if (fileMeta->chunkCnt * ChunkSize == FileSize) fileCnt++;
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