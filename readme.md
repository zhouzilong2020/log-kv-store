
# Log Key-Value Store

### Overview
In this project, we want to compare the advantage of log-based key-value store over the naive key-value store.

We will start with one key-value table setting.

The log-based key-value store will maintain a log of all changes happened to the key-value table. The log will be dumped into the disk in a manner that combines periodical and max-log-size triggered strategies. In the light of saving space both in the memory and the disk, the log will have two levels of compactions, in-memory and disk. Upon failure, the key-value table will be recovered with replaying the log stored in the disk.

In terms of naive key-value store, it is will be implemented simply using a hash table. To support durability, it will be stored by taking periodic screen shots.

We are especially interested in evaluating the throughput, memory utilization, response latency, and fault-tolerance (recovery speed and completeness) of the two approaches.
