
Experiments:

Target Scenario -> Large Scale (table size > 100,0000) KV Store

Trial experiments
1. read intensive # 0.2 - 0.8
    - random -> naive > log
    - hot key -> naive > log
    - uniform -> naive > log
2. write intensive # 0.8 - 0.2
    - random -> 
    - hot key
    - uniform
3. delete
    - compact 
4. Complexity Breakdown
    - operation
    - persist
    - compact
5. Recover
    - speed
    - mistake count
        - missing
        - unmatch
    - failure model ?
6. Memory Utilization
    - peak
    - average
7. check out redis

Extra:
1. improve naive method (possible ways)

Notes:
1. aggregate numbers

Things to Discuss:
1. naive method complexity -> write dominant (repetitive persist)

    O($N_{write}$)



2. performance & persistence tradeoff
    - naive -> table size & persist frequency
    - log -> persist frequency
3. memory & persistence tradeoff
    - naive -> less mem, long persist
    - log -> more mem, incremental persist
4. small & large scale
