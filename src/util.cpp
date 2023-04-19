/**
 * This file implements any helper functions.
 */

#include "../include/util.h"

extern std::chrono::time_point<std::chrono::high_resolution_clock> start =
    std::chrono::high_resolution_clock::now();

void *setClock()
{
    start = std::chrono::high_resolution_clock::now();
}

std::chrono::microseconds getOpTime()
{
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    return duration;
}

uint64_t getTS()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(
               now.time_since_epoch())
        .count();
}