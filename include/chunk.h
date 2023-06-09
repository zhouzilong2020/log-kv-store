#ifndef __CHUNK_H__
#define __CHUNK_H__
#include <stddef.h>
#include <util.h>

#include <cstring>

/**
 * Entry is the basic key value pair.
 */
typedef struct
{
    uint16_t version;
    uint16_t keySize;
    uint16_t valSize;
    void *payload;  // key & value
} Entry;

/**
 * Chunk is the smallest unit of memory allocation. It is managed by Log.
 */
class Chunk
{
   public:
    Chunk() { head = NULL; };
    Chunk(const int &size)
    {
        // we subtract the size of Chunk to make sure that each chunk has
        // exactly 'size' including the meta info.
        head = (char *)malloc(size - sizeof(Chunk));
        next = head;
        capacity = size - sizeof(Chunk);
        used = 0;
        entryCnt = 0;
        createdTs = getTS();
        updatedTs = createdTs;
    };
    ~Chunk()
    {
        if (head) free(head);
    };
    char *getHead() { return head; };
    int getCapacity() { return capacity; };
    int getSize() { return used; };
    int getEntryCnt() { return entryCnt; };
    int getUsed() { return used; };

    Entry *append(const Entry *entry)
    {
        const static int offset = offsetof(Entry, payload);
        int entrySize = offset + entry->keySize + entry->valSize;
        // check size, return false if there is not enough space
        if (entrySize + used > capacity)
        {
            return NULL;
        }

        Entry *newEntry = (Entry *)next;
        memcpy(next, entry, entrySize);
        next += entrySize;
        used += entrySize;
        entryCnt++;
        updatedTs = getTS();

        return newEntry;
    }

    // append appends the key-value pair to the chunk. It returns a pointer
    // points to the entry if success, NULL otherwise.
    Entry *append(const int version, const std::string &key,
                  const std::string *val)
    {
        const static int offset = offsetof(Entry, payload);
        int keySize = key.size() + 1;  // including the null char
        int valSize = val == NULL ? 0 : val->size() + 1;
        int entrySize = offset + keySize + valSize;

        // check size, return false if there is not enough space
        if (entrySize + used > capacity)
        {
            return NULL;
        }

        Entry *newEntry = (Entry *)next;
        newEntry->version = version;
        newEntry->keySize = keySize;
        newEntry->valSize = valSize;
        strncpy((char *)newEntry + offset, key.c_str(), newEntry->keySize);
        if (val)
            strncpy((char *)newEntry + offset + keySize, val->c_str(),
                    newEntry->valSize);

        updatedTs = getTS();
        // commit point begin
        next += entrySize;
        entryCnt++;
        used += entrySize;
        // commit point end

        return newEntry;
    }

   private:
    int createdTs;
    int updatedTs;
    int capacity;
    int used;
    int entryCnt;
    char *head;
    char *next;
};

#endif