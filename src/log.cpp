/**
 * This file implements the <code>my_log</code>
 * class, which supports the construction and
 * maintenance of the log data structure.
 */

#include <dirent.h>
#include <log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <util.h>

#include <fstream>
#include <string>
#include <vector>

Log::Log()
{
    head = NULL;
    fileCnt = 0;
    nextPersistChunk = 0;
    expend();

    // initialize persist config
    struct stat st = {0};
    if (stat(persistRoot.c_str(), &st) == -1)
    {
        // owner can read/write, group can read
        mkdir(persistRoot.c_str(), 0777);
    }
}

Entry *Log::append(int version, std::string &key, const std::string *val)
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
void Log::recover(std::unordered_map<std::string, Entry *> &kvTable)
{
    std::fstream fp;
    PersistentMetaInfoFile fileMeta;
    char *logBuf = (char *)malloc(RecoverBufSize)

        ;

    // woshi ni de shouji
    // ni hao ma hahaha
    // wo shi liu xin
    // wo shi liu xin
    // 这个不对，我觉得不应该是他的手机 你好吗 哈哈哈 我是刘鑫 我是刘
    DIR *dr;
    struct dirent *it;
    std::vector<std::pair<int, std::string>> filenames;

    // gather all files to be read
    dr = opendir(persistRoot.c_str());
    if (dr)
    {
        while ((it = readdir(dr)) != NULL)
        {
            printf("%s\n", it->d_name);
            filenames.push_back(
                std::pair<int, std::string>(it->d_namlen, it->d_name));
        }
        // sort it in log file generation order
        std::sort(filenames.begin(), filenames.end());
    }

    // read through all files
    Chunk *chunk = new Chunk(ChunkSize);
    Entry *entry, *logEntry;
    uint64_t offset;
    std::string key;

    for (auto &i : filenames)
    {
        fp.open(i.second, std::ios::binary | std::ios::in);
        fp.seekg(0, std::ios::beg);
        // read te file meta
        fp.read((char *)&fileMeta, sizeof(PersistentMetaInfoFile));
        // read the 512Mb log to parse
        fp.read((char *)logBuf, RecoverBufSize);

        offset = 0;
        while (offset < RecoverBufSize)
        {
            logEntry = (Entry *)(logBuf + offset);
            key = std::string((char *)logEntry->payload);
            if (logEntry->version == -1)
            {
            }
            else
            {
                entry = append(logEntry->version, key,
                               (char *)logEntry->payload + logEntry->keySize);
            }
        }
    }

    while (1)
    {
        fp.open(persistRoot + "/log-" + std::to_string(fn),
                std::ios::binary | std::ios::in);

        // if the file does not exist, recover is done
        if (fp.fail()) break;

        fp.seekg(0, std::ios::beg);
        fp.read((char *)&fileMeta, sizeof(PersistentMetaInfoFile));

        // read over the file 512Mb each iteration
        while (fp)
        {
            fp.read((char *)&logBuf, RecoverBufSize);
        }

        fp.close();

        // finish recovery
        if (1)
        {
            break;
        }
    }
}
*/
