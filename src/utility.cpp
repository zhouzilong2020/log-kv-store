/**
 * This file implements any helper functions.
 */

#include <utility.h>

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
