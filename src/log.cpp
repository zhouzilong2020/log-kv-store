/**
 * This file implements the <code>my_log</code>
 * class, which supports the construction and
 * maintenance of the log data structure.
 */

#include <log.h>
#include <unistd.h>
#include <util.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

Log::Log(std::unordered_map<std::string, Entry *> *kvTable,
         const std::string &persistRoot)
    : PersistRoot(persistRoot)
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

Log::Log(const std::string &persistRoot) : PersistRoot(persistRoot)
{
    head = NULL;
    currentFileId = 0;
    nextPersistChunk = 0;
    expend();
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
    std::cout << "log" << std::endl;
    std::cout << "Persist time: " << persistTime << std::endl;
    std::cout << "Persist duration: " << persistDuration << "ms" << std::endl;
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
    fp.open(filename);
    // write ahead
    fp.seekp(0, std::ios::end);
    fp.write((char *)head, sizeof(Chunk));
    fp.write(head->getHead(), head->getCapacity());

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
