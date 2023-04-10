/**
 * This file is used to launch the experiments.
 */

// TODO: correct the include path
#include <log.h>
#include <log_kv.h>
#include <naive_kv.h>

#include <iostream>
#include <string>
#include <unordered_map>

std::string randomString(int strLen)
{
    std::string str;
    str.reserve((size_t)strLen);

    for (int i = 0; i < strLen; i++)
    {
        char charASCII = 'a' + arc4random() % 24;
        str.append(&charASCII);
    }

    return str;
}

bool cmpTables(LogKV &logKV, std::unordered_map<std::string, std::string> &map)
{
    assert(logKV.size() == map.size());
    for (auto kv : map)
    {
        if (kv.second != logKV.get(kv.first))
        {
            printf("wrong val\n");
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
        printf("put %s %s\n", key.c_str(), val.c_str());

        logKV.put(key, val);
    }

    if (!cmpTables(logKV, map))
    {
        exit(1);
    }

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
        printf("put %s %s\n", key.c_str(), val.c_str());
    }

    std::vector<std::string>::iterator it;

    // remove first
    it = allKey.begin();
    printf("delete %s %s\n", (*it).c_str(), (map[*it]).c_str());
    map.erase(*it);
    logKV.deleteK(*it);
    allKey.erase(it);
    if (!cmpTables(logKV, map)) exit(1);

    // remove last
    it = allKey.end() - 1;
    printf("delete %s %s\n", (*it).c_str(), (map[*it]).c_str());
    map.erase(*it);
    logKV.deleteK(*it);
    allKey.erase(it);
    if (!cmpTables(logKV, map)) exit(1);

    // remove mid
    it = allKey.begin() + 5;
    printf("delete %s %s\n", (*it).c_str(), (map[*it]).c_str());
    map.erase(*it);
    logKV.deleteK(*it);
    allKey.erase(it);
    if (!cmpTables(logKV, map)) exit(1);

    // remove all
    for (it = allKey.begin(); it < allKey.end(); it++)
    {
        printf("delete %s %s\n", (*it).c_str(), (map[*it]).c_str());
        map.erase(*it);
        logKV.deleteK(*it);
        if (!cmpTables(logKV, map)) exit(1);
    }

    printf("Succeed!\n");
}

int main(int argc, char *argv[])
{
    try
    {
        testBasicGetPut();
        testBasicDelete();
    }
    catch (const std::exception &ex)
    {
        fprintf(stderr, "%s", ex.what());
    }
    return 0;
}
