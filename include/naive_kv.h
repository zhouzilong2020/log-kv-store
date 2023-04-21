/**
 * This file defines the <code>naive_kv</code>
 * class.
 */

#ifndef __NAIVE_KV_H__
#define __NAIVE_KV_H__
#include <string>
#include <unordered_map>

#include "kv_store.h"

class NaiveKV : public KVStore
{
   public:
    virtual int put(const std::string &key, const std::string *val) override;

    virtual std::unique_ptr<std::string> get(const std::string &key) override;

    virtual void deleteK(const std::string &key) override;

    // do nothing
    ~NaiveKV(){};

   private:
    // this map stores the current in memory key-value table
    std::unordered_map<std::string, std::string> kvTable;
};
#endif