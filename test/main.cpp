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

    for (auto kv : map)
    {
        if (kv.second != logKV.get(kv.first))
        {
            printf("wrong val\n");
            exit(1);
        }
    }
    printf("Succeed!\n");
}

void testBasicDelete()
{
    LogKV logKV;
    std::unordered_map<std::string, std::string> map;

    for (int i = 0; i < 10; i++)
    {
        std::string key = randomString(25);
        std::string val = randomString(50);

        map[key] = val;
        logKV.put(key, val);
    }

    for (auto kv : map)
    {
        assert(kv.second == logKV.get(kv.first));
    }
}

int main(int argc, char *argv[])
{
    try
    {
        testBasicGetPut();
    }
    catch (const std::exception &ex)
    {
        fprintf(stderr, "%s", ex.what());
    }
    return 0;
}
