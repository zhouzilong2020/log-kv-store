/**
 * This file implements any helper functions.
 */

#include <util.h>

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

std::string getTimeFormat(const std::string &format)
{
    time_t now = time(0);
    struct tm tStruct;
    char buf[80];
    tStruct = *localtime(&now);
    strftime(buf, sizeof(buf), format.c_str(), &tStruct);
    return buf;
}