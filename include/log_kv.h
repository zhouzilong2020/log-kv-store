/**
 * This file defines the <code>log_kv</code>
 * class.
*/

#include <unordered_map>
#include <string.h>
#include <log.h>

class log_kv {
    private:
        std::unordered_map<std::string, std::string> kv_table;  // this map stores the current key-value table
        my_log kv_log;      // data structure that maintains the log

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


