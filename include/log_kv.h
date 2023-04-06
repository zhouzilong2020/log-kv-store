/**
 * This file defines the <code>log_kv</code>
 * class.
 */

#ifndef __LOG_KV_H__
#define __LOG_KV_H__
#include <log.h>

#include <string>
#include <unordered_map>

class log_kv {
   private:
    // this map stores the current key-value table
    std::unordered_map<std::string, std::string> kv_table;
    // data structure that maintains the log
    my_log kv_log;

   public:
    /**
     * This function replaus the disk log to reconstruct the
     * key-value table, after a failure.
     */
    int recover();

    /**
     * Client can use <code>put</code> to update
     * an key-value pair in the table
     */
    int put(std::string key, std::string value, int version);

    /**
     * Client can use <code>get</code> to get
     * the current value of the given key.
     */
    int get(std::string key);

    /**
     * Client can use <code>delete_k</code> to
     * delete the key-value pair from the table.
     */
    int delete_k(std::string key);
};

#endif