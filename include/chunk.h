#ifndef __CHUNK_H__
#define __CHUNK_H__
#include <stddef.h>
#include <util.h>

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

enum chunkMeta
{
    CREATEDTS,
    UPDATEDTS,
    CAPACITY,
    USED,
    ENTRYCNT
};


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
        strlcpy((char *)&newEntry->payload, key.c_str(), newEntry->keySize);
        if (val)
            strlcpy((char *)&newEntry->payload + keySize, val->c_str(),
                    newEntry->valSize);
        // head = (char *)head + entrySize;

        updatedTs = getTS();
        // commit point begin
        next += entrySize;
        entryCnt++;
        used += entrySize;
        // commit point end

        return newEntry;
    }

    int get(chunkMeta target)
    {
        switch (target)
        {
        case CREATEDTS:
            return createdTs;
        case UPDATEDTS:
            return updatedTs;
        case CAPACITY:
            return capacity;
        case USED:
            return used;
        case ENTRYCNT:
            return entryCnt;
        }
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