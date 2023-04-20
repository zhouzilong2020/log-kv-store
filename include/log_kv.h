/**
 * This file defines the <code>log_kv</code>
 * class.
 */

#ifndef __LOG_KV_H__
#define __LOG_KV_H__
#include <log.h>

#include <string>
#include <unordered_map>

#include "kv_store.h"

class LogKV : public KVStore
{
   public:
    LogKV();
    ~LogKV()
    {
        if (log) delete log;
    };

    /**
     * This function replaus the disk log to reconstruct the
     * key-value table, after a failure.
     */
    int recover();

    /**
     * Client can use <code>put</code> to update
     * an key-value pair in the table
     * val is nullable so we use pointer here.
     */
    virtual int put(const std::string &key, const std::string *val) override;

    /**
     * Client can use <code>get</code> to get
     * the current value of the given key.
     */
    virtual std::unique_ptr<std::string> get(const std::string &key) override;

    /**
     * Client can use <code>delete_k</code> to
     * delete the key-value pair from the table.
     */
    virtual void deleteK(const std::string &key) override;

    /**
     * size returns the current size of the kv
     * store, which equals to the number of keys
     * currently in the table
     */
    size_t size();

    void tryCompact()
    {
        if (duplicatedEntryCnt < (1 << 15))
        {
            return;
        }
        Log *compactedLog = new Log(&kvTable);
        if (log) delete log;
        log = compactedLog;
        duplicatedEntryCnt = 0;
    }

   private:
    // this map stores the current key-value table
    std::unordered_map<std::string, Entry *> kvTable;
    // data structure that maintains the log
    Log *log;
    size_t tableSize;
    int duplicatedEntryCnt;
};

#endif