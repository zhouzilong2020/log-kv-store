# Log Key-Value Store

## Road Map
- [x] :bangbang: Basic Data structure 
- [x] :bangbang: Comparison between std::unordered_map and ours
- [x] :bangbang: Persist method
- [ ] :heavy_exclamation_mark: Snapshot for fast recovery
- [ ] :interrobang: Point in Time Recovery

## Overview
In this project, we want to compare the advantage of log-based key-value store over the naive key-value store.

We will start with one key-value table setting.

<img src="fig/log-structred.jpeg">

The log-based key-value store will maintain a log of all changes happened to the key-value table. The log will be dumped into the disk in a manner that combines periodical and max-log-size triggered strategies. In the light of saving space both in the memory and the disk, the log will have two levels of compactions, in-memory and disk. Upon failure, the key-value table will be recovered with replaying the log stored in the disk.

In terms of naive key-value store, it is will be implemented simply using a hash table. To support durability, it will be stored by taking periodic screen shots.

We are especially interested in evaluating the throughput, memory utilization, response latency, and fault-tolerance (recovery speed and completeness) of the two approaches.

## The Goal of this project
We try to examine the differences between log-structure kv store with traditional kv store design like std::unordered_map in the following aspects:
- Performance (Single W/R or concurrent W/R).
- persistency (How long does it take to persist to non-volatile storage).
- Time to Recovery from failure.
- Data lost rate when failure happens.


## Why log-kv
Each entry will only be appended to the log, which may promise good performance under high concurrency. With log-structure, we may bulk allocate memory instead of allocate small chunk for many times, which may bottleneck the overall performance.

## Supported data
- key: random string at most 64 Bytes.
- value: random string at most 1Mb.

## Supported operations
- Put(k, v)
- Get(k) 
- Delete(k)

## Data Model
In our LogKV, the in memory hash table stores the key and a pointer pointing to the corresponding entry in the chunk. The chunk is the basic memory management and persistent unit. We append a chunk into the file once it is full and allocate a new chunk for further logging.

<img src="fig/log-structure.png">
<img src="fig/data-modelv2.png">

## How to start?
```bash
git clone git@github.com:zhouzilong2020/log-kv-store.git
# cd to the root dir of the project
make               # compile 
chmod +x ./log-kv  # grant execution to the executable
./log-kv -t log    # start the service in log mode
```
```bash
scripts/test c     # start the cartesian testing described in our report
scripts/test e     # start the example testing described in our report
```
```bash
scripts/fig        # draw the figures shown in our report
```
