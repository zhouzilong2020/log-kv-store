/**
 * This file implements any helper functions.
 */

#include "../include/util.h"

extern std::chrono::time_point<std::chrono::high_resolution_clock> start =
    std::chrono::high_resolution_clock::now();

void* setClock()
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

void removeDir(const char* path)
{
    DIR* dir = opendir(path);
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        const char* name = entry->d_name;

        // Skip the "." and ".." entries
        if (name[0] == '.' &&
            (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
        {
            continue;
        }

        char* sub_path = (char*)malloc(strlen(path) + strlen(name) + 2);
        sprintf(sub_path, "%s/%s", path, name);

        if (entry->d_type == DT_DIR)
        {
            // Recursive call to remove subdirectory
            removeDir(sub_path);
        }
        else
        {
            // Remove regular file
            if (remove(sub_path) != 0)
            {
                perror("Error deleting file");
                exit(EXIT_FAILURE);
            }
        }

        free(sub_path);
    }

    closedir(dir);

    if (rmdir(path) != 0)
    {
        perror("Error deleting directory");
        exit(EXIT_FAILURE);
    }
}
