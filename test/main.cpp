#include <getopt.h>
#include <log.h>
#include <log_kv.h>
#include <naive_kv.h>
#include <util.h>

#include <iostream>
#include <string>
#include <unordered_map>

using namespace std::chrono;

std::string randomString(uint strLen)
{
    std::string str;
    str.reserve((size_t)strLen);

    for (int i = 0; i < static_cast<int>(strLen); i++)
    {
        char charASCII = 'a' + arc4random() % 26;
        str.push_back(charASCII);
    }

    return str;
}

bool cmpTables(LogKV &logKV, std::unordered_map<std::string, std::string> &map)
{
    assert(logKV.size() == map.size());
    int cnt = 0;
    for (auto kv : map)
    {
        auto ptr = logKV.get(kv.first);
        if (ptr == nullptr || kv.second != *ptr)
        {
            fprintf(stderr,
                    "wrong at %d key\n%s\nexpected (%lu)\n%s\ngot (%lu)\n%s\n",
                    cnt, kv.first.c_str(), kv.second.size(), kv.second.c_str(),
                    ptr->size(), ptr->c_str());
            return false;
        }
        cnt++;
    }
    return true;
}

bool cmpLogKVs(LogKV &refLogKV, LogKV &newLogKV)
{
    const std::vector<Chunk *> *refChunkVec = refLogKV.getChunkList();
    const std::vector<Chunk *> *newChunkVec = newLogKV.getChunkList();
    Chunk *refChunk;
    Chunk *newChunk;
    char *refHead;
    char *newHead;
    Entry *refEntry;
    Entry *newEntry;
    uint64_t entryOffset;

    const static uint64_t payloadOffset = offsetof(Entry, payload);

    // newChunkVec->size() because the newest chunk in previous log might be
    // lost
    for (int i = 0; i < static_cast<int>(newChunkVec->size()); i++)
    {
        refChunk = (*refChunkVec)[i];
        newChunk = (*newChunkVec)[i];
        if (refChunk->get(CAPACITY) != newChunk->get(CAPACITY) ||
            refChunk->get(USED) != newChunk->get(USED) ||
            refChunk->get(ENTRYCNT) != newChunk->get(ENTRYCNT))
        {
            printf("CAPACITY: %d %d\n", refChunk->get(CAPACITY),
                   newChunk->get(CAPACITY));
            printf("USED: %d %d\n", refChunk->get(USED), newChunk->get(USED));
            printf("ENTRYCNT: %d %d\n", refChunk->get(ENTRYCNT),
                   newChunk->get(ENTRYCNT));
            exit(1);
        }

        refHead = refChunk->getHead();
        newHead = newChunk->getHead();
        entryOffset = 0;

        for (int cnt = 0; cnt < refChunk->get(ENTRYCNT); cnt++)
        {
            refEntry = (Entry *)(refHead + entryOffset);
            newEntry = (Entry *)(newHead + entryOffset);

            if (refEntry->version != newEntry->version ||
                refEntry->keySize != newEntry->keySize ||
                refEntry->valSize != newEntry->valSize ||
                strcmp((char *)&refEntry->payload,
                       (char *)&newEntry->payload) != 0 ||
                strcmp((char *)&refEntry->payload + refEntry->keySize,
                       (char *)&newEntry->payload + newEntry->keySize) != 0)
            {
                printf("Entry num %d\n", cnt);
                printf("version: %d %d\n", refEntry->version,
                       newEntry->version);
                assert(refEntry->version == newEntry->version);

                printf("keySize: %d %d\n", refEntry->keySize,
                       newEntry->keySize);
                assert(refEntry->keySize == newEntry->keySize);

                printf("valSize: %d %d\n", refEntry->valSize,
                       newEntry->valSize);
                assert(refEntry->valSize == newEntry->valSize);

                printf("key: %s %s\n", (char *)&refEntry->payload,
                       (char *)&newEntry->payload);
                printf("key length: %lu %lu\n",
                       strlen((char *)&refEntry->payload),
                       strlen((char *)&newEntry->payload));
                assert(strcmp((char *)&refEntry->payload,
                              (char *)&newEntry->payload));

                printf("val: %s %s\n",
                       (char *)&refEntry->payload + refEntry->keySize,
                       (char *)&newEntry->payload + newEntry->keySize);
                assert(strcmp((char *)&refEntry->payload + refEntry->keySize,
                              (char *)&newEntry->payload + newEntry->keySize));
                exit(1);
            }

            entryOffset +=
                payloadOffset + refEntry->keySize + refEntry->valSize;
        }
    }

    return true;
}

void testBasicGetPut()
{
    printf("testing testBasicGetPut\n");

    LogKV logKV;
    std::unordered_map<std::string, std::string> map;

    for (int i = 0; i < 10000; i++)
    {
        std::string key = randomString(25);
        std::string val = randomString(50);
        map[key] = val;
        logKV.put(key, &val);
    }

    assert(cmpTables(logKV, map));
    printf("Succeed!\n");
}

void testBasicDelete()
{
    printf("testing testBasicDelete\n");

    LogKV logKV;
    std::unordered_map<std::string, std::string> map;
    std::vector<std::string> allKey;

    for (int i = 0; i < 10000; i++)
    {
        std::string key = randomString(25);
        std::string val = randomString(50);

        map[key] = val;
        logKV.put(key, &val);
        allKey.push_back(key);
    }

    std::vector<std::string>::iterator it;

    // remove first
    it = allKey.begin();
    map.erase(*it);
    logKV.deleteK(*it);
    allKey.erase(it);
    assert(cmpTables(logKV, map));

    // remove last
    it = allKey.end() - 1;
    map.erase(*it);
    logKV.deleteK(*it);
    allKey.erase(it);
    assert(cmpTables(logKV, map));

    // remove mid
    it = allKey.begin() + 5;
    map.erase(*it);
    logKV.deleteK(*it);
    allKey.erase(it);
    assert(cmpTables(logKV, map));

    // randomly remove all
    for (uint i = 0, len = allKey.size(); i < len; i++)
    {
        it = allKey.begin() + arc4random() % allKey.size();
        map.erase(*it);
        logKV.deleteK(*it);
        assert(logKV.get(*it) == nullptr);
        allKey.erase(it);
    }

    printf("Succeed!\n");
}

void testAdvanced()
{
    printf(
        "testing testAdvanced (512K short kv with 0.1 deletes 0.2 updates)\n");

    LogKV logKV;
    std::unordered_map<std::string, std::string> map;
    std::vector<std::string> keys;

    // 512K entries
    for (int i = 0; i < (1 << 19); i++)
    {
        std::string key = randomString(10);
        std::string val = randomString(20);
        keys.push_back(key);
        map[key] = val;
        logKV.put(key, &val);

        // 10% chance to delete
        if (arc4random() % 100 < 10)
        {
            int idx = arc4random() % keys.size();
            auto key = keys[idx];
            keys.erase(keys.begin() + idx);
            map.erase(key);
            logKV.deleteK(key);
        }

        // 20% chance to update
        if (arc4random() % 100 < 20)
        {
            int idx = arc4random() % keys.size();
            auto key = keys[idx];
            std::string newVal = randomString(20);
            logKV.put(key, &newVal);
            map[key] = newVal;
        }
    }

    assert(cmpTables(logKV, map));
    printf("Succeed!\n");
}

void testBigKV()
{
    printf("testing testBigKV\n");

    LogKV logKV;
    std::unordered_map<std::string, std::string> map;

    // 1k entries
    for (int i = 0; i < (1 << 10); i++)
    {
        std::string key = randomString(200);
        std::string val = randomString(5000);
        map[key] = val;
        logKV.put(key, &val);
    }

    assert(cmpTables(logKV, map));
    printf("Succeed!\n");
}


void testRecoverDbg()
{
    printf("testing testRecoverDbg\n");

    LogKV logKV;
    std::unordered_map<std::string, std::string> map;
    std::string key;
    std::string val;

    for (int i = 0; i < (1 << 18); i++)
    {
        key = std::string("key") + std::to_string(i);
        val = std::string("value") + std::to_string(i);
        map[key] = val;
        logKV.put(key, &val);
    }
    cmpTables(logKV, map);

    LogKV newLogKV;
    newLogKV.recover();

    cmpLogKVs(logKV, newLogKV);
    printf("Succeed!\n");
}

void testRecoverBig()
{
    printf("testing testRecoverBig\n");

    LogKV logKV;
    std::unordered_map<std::string, std::string> map;
    std::string key;
    std::string val;

    for (int i = 0; i < (1 << 18); i++)
    {
        std::string key = randomString(20);
        std::string val = randomString(50);
        map[key] = val;
        logKV.put(key, &val);
    }
    cmpTables(logKV, map);

    LogKV newLogKV;
    newLogKV.recover();

    cmpLogKVs(logKV, newLogKV);
    printf("Succeed!\n");
}

void testPersist()
{
    printf("testing testPersist\n");

    LogKV logKV;
    std::unordered_map<std::string, std::string> map;

    // big enough to trigger persist
    for (int i = 0; i < 1 << 17; i++)
    {
        std::string key = randomString(10);
        std::string val = randomString(20);
        map[key] = val;
        logKV.put(key, &val);
    }

    assert(cmpTables(logKV, map));
    printf("Succeed!\n");
}

void runTest()
{
    testBasicGetPut();
    removeDir(".persist");

    testBasicDelete();
    removeDir(".persist");

    testAdvanced();
    removeDir(".persist");

    testBigKV();
    removeDir(".persist");
    
    testRecoverDbg();
    removeDir(".persist");
    
    testRecoverBig();
    removeDir(".persist");

    testPersist();
    removeDir(".persist");
}

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"type", required_argument, 0, 't'},
    {"test", optional_argument, 0, 'T'},
    {0, 0, 0, 0}  // indicate the end of the array
};

void print_usage()
{
    printf("Usage: log_kv [OPTIONS]\n");
    printf("Options:\n");
    printf("  -h, --help   %s\n", "Display this help message");
    printf("  -t, --type   %s\n",
           "Choose the type of KVStore, possible options: [naive/log]");
    printf("  -T, --test   %s\n", "Run build in test");
}

int main(int argc, char **argv)
{
    int option_index = 0;
    int c;
    KVStore *kv = NULL;
    while ((c = getopt_long(argc, argv, "ht:T", long_options, &option_index)) !=
           -1)
    {
        switch (c)
        {
        case 'h':
        case '?':
            print_usage();
            exit(EXIT_SUCCESS);
        case 't':
            // Handle input option
            if (strcmp(optarg, "naive") == 0)
                kv = new NaiveKV();
            else if (strcmp(optarg, "log") == 0)
                kv = new LogKV();
            else
            {
                print_usage();
                exit(EXIT_SUCCESS);
            }
            break;
        case 'T':
            runTest();
            exit(EXIT_SUCCESS);
        default:
            print_usage();
            exit(EXIT_SUCCESS);
        }
    }

    if (kv)
    {
        kv->run();
        delete kv;
    }
}