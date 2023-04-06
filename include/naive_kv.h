/**
 * This file defines the <code>naive_kv</code>
 * class.
 */

#ifndef __NAIVE_KV_H__
#define __NAIVE_KV_H__
#include <string>
#include <unordered_map>

class NaiveKV
{
   public:
    /**
     * takeSnapshot take a snapshot of the current
     * key-value table into the disk.
     */
    int takeSnapshot();

    /**
     * recover recover the key-value table using
     * the disk snapshot, after a failure.
     */
    int recover();

    /**
     * put put the key-value pair into the hash table
     */
    int put(std::string key, std::string value, int version);

    /**
     * get return the value of the given key
     */
    int get(std::string key);

    /**
     * deleteK delete the given key-value pair
     */
    int deleteK(std::string key);

   private:
    // this map stores the current in memory key-value table
    std::unordered_map<std::string, std::string> kvTable;
};
#endif