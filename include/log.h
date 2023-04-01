/**
 * This file defines the <code>my_log</code>
 * class.
 */

/**
 * Each entry in the log will be composed of
 * four attributes.
 */
struct entry {
    int version;
    long key_size;
    long val_size;
    void *payload;
};

class my_log {
   private:
    void *log_head;      // points to the start of first log segment
    void *log_seg_head;  // points to the first writable byte of the current log
    long size;           // record the number of entries currently in the log
    long byte_size;      // log size in bytes
    int write_interval;  // specifies the time interval that a disk write will
                         // be triggered

    int expend();      // expend the log
    int compact();     // compact the log, removing unnecessary entries
    int write_disk();  // write the current log into the disk
    int
    timer_trigger();  // this function will trigger the disk write periodically,
                      // even if the log capacity is not reached

   public:
    /**
     * This function append the key-value pair,
     * together with the version number of the event
     * into the log.
     *
     * Note that a -1 version number stands for a deletion.
     */
    void append(char *key, char *value, int version);

    /**
     * This function load the log from the disk to recover
     * the in-memory log, after a failure.
     */
    int recover();
};
