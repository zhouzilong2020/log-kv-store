/**
 * This file is used to launch the experiments.
 */

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

    for (int i = 0; i < strLen; i++)
    {
        char charASCII = 'a' + arc4random() % 26;
        str.append(&charASCII);
    }

    return str;
}

bool cmpTables(LogKV &logKV, std::unordered_map<std::string, std::string> &map)
{
    assert(logKV.size() == map.size());
    for (auto kv : map)
    {
        auto ptr = logKV.get(kv.first);
        if (ptr == nullptr || kv.second != *ptr)
        {
            fprintf(stderr, "key\n%s\nexpected (%lu)\n%s\ngot (%lu)\n%s\n",
                    kv.first.c_str(), kv.second.size(), kv.second.c_str(),
                    ptr->size(), ptr->c_str());
            return false;
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
    printf("testing testAdvanced (2M kv with 0.1 deletes 0.2 updates)\n");

    LogKV logKV;
    std::unordered_map<std::string, std::string> map;
    std::vector<std::string> keys;

    // 2M entries
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
            std::string newVal = randomString(50);
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

void runTest()
{
    testBasicGetPut();
    testBasicDelete();
    testAdvanced();
    testBigKV();
}

int main(int argc, char **argv)
{
    runTest();
    return 0;
}