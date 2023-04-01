/**
 * This file defines the <code>naive_kv</code>
 * class.
 */

#include <string.h>

#include <unordered_map>

class naive_kv {
   private:
    // this map stores the current in memory key-value table
    std::unordered_map<std::string, std::string> kv_table;

   public:
    /**
     * This function writes a snapshot of the current
     * key-value table into the disk.
     */
    int take_snapshot12312();

    /**
     * This function recover the key-value table using
     * the disk snapshot, after a failure.
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
