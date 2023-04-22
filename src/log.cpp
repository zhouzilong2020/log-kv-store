/**
 * This file implements the <code>my_log</code>
 * class, which supports the construction and
 * maintenance of the log data structure.
 */

#include <log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <util.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

Log::Log(std::unordered_map<std::string, Entry *> *kvTable)
{
    static int cnt = 0;
    cnt++;

    head = NULL;
    currentFileId = 0;
    nextPersistChunk = 0;
    expend(false);
    for (auto &kv : *kvTable)
    {
        auto newEntry = head->append(kv.second);
        if (!newEntry)
        {
            expend(false);
            newEntry = head->append(kv.second);
        }
        kv.second = newEntry;
    }

    recovering = false;
    // persist();
}

Log::Log()
{
    head = NULL;
    currentFileId = 0;
    nextPersistChunk = 0;
    expend();

    // initialize persist directory
    if (!existDir(PersistRoot.c_str()))
    {
        // owner can read/write, group can read
        mkdir(PersistRoot.c_str(), 0777);
    }

    // initialize non-recovering log
    recovering = false;
}
Log::~Log()
{
    // FIXME:partial persist
    // persist();
    for (auto chunk : chunkList)
    {
        if (chunk) delete chunk;
    }
}

Entry *Log::append(const int version, const std::string &key,
                   const std::string *val)
{
    Entry *entryPtr = head->append(version, key, val);
    if (entryPtr == NULL)  // the current chunk is full
    {
        expend();
        // this time it should work
        entryPtr = head->append(version, key, val);
    }
    return entryPtr;
}

/*
 * persist writes the current log to the disk.
 */
int Log::persist()
{
    std::string filename =
        PersistRoot + "/log-" + std::to_string(currentFileId);
    MetaInfoPersistentFile *fileMeta = NULL;
    // create if not exist, otherwise open
    std::fstream fp;
    fp.open(filename, std::ios::app);
    if (fp.tellg() == 0)  // newly created file
    {
        // init file meta info
        fileMeta = new MetaInfoPersistentFile();
        fp.write((char *)fileMeta, sizeof(MetaInfoPersistentFile));
        persistentFileTable[filename] = fileMeta;
    }
    else
    {
        fileMeta = persistentFileTable[filename];
    }
    fp.close();
    // printf("Persist file header %llu %llu %llu %llu\n", fileMeta->createdTs,
    //        fileMeta->updatedTs, fileMeta->chunkCnt, fileMeta->tail);

    fp.open(filename);
    // fp.seekg(0, std::ios::beg);
    // fp.read((char *)fileMeta, sizeof(MetaInfoPersistentFile));
    // write ahead
    fp.seekp(0, std::ios::end);
    fp.write((char *)head, sizeof(Chunk));
    fp.write(head->getHead(), head->getCapacity());
    // printf(
    //     "Persist chunk info %d %d %d %d %d | payload offset"
    //     "%p\n",
    //     head->get(CREATEDTS), head->get(UPDATEDTS), head->get(ENTRYCNT),
    //     head->get(CAPACITY), head->get(USED), head->getHead());

    fileMeta->addChunk(ChunkSize);
    fp.seekp(0, std::ios::beg);
    // commit begins
    fp.write((char *)fileMeta, sizeof(MetaInfoPersistentFile));
    fp.close();
    // commit ends

    nextPersistChunk++;
    // persist to another file if the current file is full
    if (fileMeta->chunkCnt * ChunkSize == FileSize) currentFileId++;
    return 0;
}

void Log::hideFile()
{
    for (auto &kv : persistentFileTable)
    {
        std::string newName = kv.first + ".hide";
        if (std::rename(kv.first.c_str(), newName.c_str()) != 0)
        {
            std::perror("Failed to hide old file");
        }
    }
}

void Log::removePersistedFile()
{
    for (auto &kv : persistentFileTable)
    {
        std::string filename = kv.first + ".hide";
        if (std::remove(filename.c_str()) != 0)
        {
            std::perror("Failed to remove file");
        }
    }
}
