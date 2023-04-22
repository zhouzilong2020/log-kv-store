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
    // do nothing
    ~NaiveKV(){};
    virtual void put(const std::string &key, const std::string *val) override;
    virtual std::unique_ptr<std::string> get(const std::string &key) override;
    virtual void deleteK(const std::string &key) override;
    virtual void persist() override;
    virtual void recover() override;

   private:
    // this map stores the current in memory key-value table
    std::unordered_map<std::string, std::string> kvTable;
    const std::string persistFile = "./.persist/naive_kv";
    uint byteSize = 0;
};
#endif