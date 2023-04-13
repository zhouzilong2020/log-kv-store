/**
 * This file is used to launch the experiments.
 */

#include <log.h>
#include <log_kv.h>
#include <naive_kv.h>
#include <utility.h>

#include <iostream>
#include <string>
#include <unordered_map>

using namespace std::chrono;

std::string randomString(int strLen)
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
            std::cerr << "Wrong val, expected\n"
                      << kv.second << "\ngot\n"
                      << *ptr << std::endl;
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

    for (int i = 0; i < 10; i++)
    {
        std::string key = randomString(25);
        std::string val = randomString(50);
        map[key] = val;
        logKV.put(key, val);
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

    for (int i = 0; i < 10; i++)
    {
        std::string key = randomString(25);
        std::string val = randomString(50);

        map[key] = val;
        logKV.put(key, val);
        allKey.push_back(key);
        // printf("put %s %s\n", key.c_str(), val.c_str());
    }

    std::vector<std::string>::iterator it;

    // remove first
    it = allKey.begin();
    // printf("delete %s %s\n", (*it).c_str(), (map[*it]).c_str());
    map.erase(*it);
    logKV.deleteK(*it);
    allKey.erase(it);
    assert(cmpTables(logKV, map));

    // remove last
    it = allKey.end() - 1;
    // printf("delete %s %s\n", (*it).c_str(), (map[*it]).c_str());
    map.erase(*it);
    logKV.deleteK(*it);
    allKey.erase(it);
    assert(cmpTables(logKV, map));

    // remove mid
    it = allKey.begin() + 5;
    // printf("delete %s %s\n", (*it).c_str(), (map[*it]).c_str());
    map.erase(*it);
    logKV.deleteK(*it);
    allKey.erase(it);
    assert(cmpTables(logKV, map));

    // remove all
    for (it = allKey.begin(); it < allKey.end(); it++)
    {
        // printf("delete %s %s\n", (*it).c_str(), (map[*it]).c_str());
        map.erase(*it);
        logKV.deleteK(*it);
        assert(cmpTables(logKV, map));
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
    for (int i = 0; i < (1 << 20); i++)
    {
        std::string key = randomString(25);
        std::string val = randomString(50);
        keys.push_back(key);
        map[key] = val;
        logKV.put(key, val);

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
            logKV.put(key, newVal);
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
        logKV.put(key, val);
    }

    assert(cmpTables(logKV, map));

    printf("Succeed!\n");
}

int main(int argc, char *argv[])
{
    testBasicGetPut();
    testBasicDelete();
    testAdvanced();
    testBigKV();

    return 0;
}
