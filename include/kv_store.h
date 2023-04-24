/**
 * This file defines the <code>naive_kv</code>
 * class.
 */

#ifndef __KV_STORE_H__
#define __KV_STORE_H__
#include <sys/stat.h>
#include <sys/types.h>
#include <util.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class KVStore
{
   public:
    void run()
    {
        std::string line;
        // Loop to read input until end of file is reached.
        while (std::getline(std::cin, line))
        {
            std::vector<std::string> args = split(line, ' ');
            // convert to lower case
            std::transform(args[0].begin(), args[0].end(), args[0].begin(),
                           [](unsigned char c) { return std::tolower(c); });
            if (args[0] == CMD_PUT)
            {
                if (args.size() != 3) continue;
                put(args[1], &args[2]);
            }
            else if (args[0] == CMD_GET)
            {
                if (args.size() != 2) continue;
                auto res = this->get(args[1]);
                // if the key exists, print the value
                if (res) std::cout << *res << std::endl;
            }
            else if (args[0] == CMD_DELETE)
            {
                if (args.size() != 2) continue;
                deleteK(args[1]);
            }
            else if (args[0] == CMD_STATISTIC)
            {
                statistic();
            }
        }
    }

    KVStore()
    {
        // initialize persist directory
        if (!existDir(PersistRoot.c_str()))
        {
            // owner can read/write, group can read
            mkdir(PersistRoot.c_str(), 0777);
        }
    };
    virtual ~KVStore(){};
    // put creates or updates a key-value pair in the table. val is nullable so
    // we use pointer here.
    virtual void put(const std::string &key, const std::string *val) = 0;
    // get gets the corresponding value to the given key, return NULL otherwise.
    virtual std::unique_ptr<std::string> get(const std::string &key) = 0;
    // deleteK deletes the key-value pair from the table.
    virtual void deleteK(const std::string &key) = 0;
    // persist persists the current key-value table to disk.
    virtual void persist() = 0;
    //  recover replays the disk log to reconstruct the key-value table, after a
    //  failure.
    virtual void recover() = 0;
    // statistic prints the breakdown time of kernel operations.
    // (i.e. how many time is spent on each persist, new chunk, malloc, etc.)
    void statistic()
    {
        std::cout << "Persist time: " << persistTime << std::endl;
        std::cout << "Persist duration: " << persistDuration << "ms"
                  << std::endl;
    };

   protected:
    // TODO: make it configurable
    const std::string PersistRoot = "./.persist";
    uint persistTime = 0;
    long long persistDuration = 0;

   private:
    const std::string CMD_PUT = "put";
    const std::string CMD_GET = "get";
    const std::string CMD_DELETE = "delete";
    const std::string CMD_STATISTIC = "statistic";

    std::vector<std::string> split(const std::string &str, char delimiter)
    {
        std::vector<std::string> words;
        std::stringstream ss(str);
        std::string word;
        while (std::getline(ss, word, delimiter))
        {
            words.push_back(word);
        }
        return words;
    }
};
#endif