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

const static char *RemovePrompt =
    "\n### Do you want to remove persist logs? (YES/ANYTHING) >>> ";

class LogKV : public KVStore
{
   public:
    LogKV();
    ~LogKV();

    /**
     * This function replaus the disk log to reconstruct the
     * key-value table, after a failure.
     */
    int recover();

    /**
     * failure simulates a machine failure by
     */
    void failure();

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


    const std::vector<Chunk *> *getChunkList() { return log->getChunkList(); }

    void tryCompact()
    {
        double currentUsg = double(log->currentChunkUsed()) /
                            double(log->currentChunkCapacity());
        if (duplicatedEntryCnt < (1 << 15) || currentUsg < 0.8)
        {
            return;
        }
        Log *compactedLog = new Log(&kvTable);

        // make old persistent file hidden
        log->hideFile();
        // create new persistent file
        compactedLog->persist();
        log->removePersistedFile();
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

    const uint64_t RecoverBufSize = 512 * (1 << 20);  // 512Mb recover buf

    /**
     * replayEntry replays the given log slice
     */
    void replayChunk(Chunk *chunk, char *chunkPayload)
    {
        const static int payloadOffset = offsetof(Entry, payload);
        Entry *logEntry;
        uint64_t entryOffset = 0;
        std::string key;
        std::string val;
        int entryCnt = 0;

        while (entryCnt < chunk->get(ENTRYCNT))
        {
            /** TODO: is copy one-by-one a good idea?  */
            // printf("entryOffset %lld\n", entryOffset);
            logEntry = (Entry *)(chunkPayload + entryOffset);
            // if (entryCnt == 0)
            //     printf("Recover Entry info %d %d %d | %s <-> %s\n",
            //            logEntry->version, logEntry->keySize,
            //            logEntry->valSize, (char *)&logEntry->payload, (char
            //            *)&logEntry->payload + logEntry->keySize);

            key = std::string((char *)&logEntry->payload);
            val = std::string((char *)&logEntry->payload + logEntry->keySize);

            if (logEntry->version == std::numeric_limits<uint16_t>::max())
            {
                deleteK(key);
            }
            else
            {
                put(key, &val);
            }

            entryOffset +=
                payloadOffset + logEntry->keySize + logEntry->valSize;
            entryCnt++;
        }
    }

};

#endif