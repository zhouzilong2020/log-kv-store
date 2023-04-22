/**
 * This file implements any helper functions.
 */

#include "../include/util.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

uint64_t getTS()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(
               now.time_since_epoch())
        .count();
}

void listDir(const char *path, std::vector<std::string> &files)
{
    DIR *dr;
    struct dirent *it;

    std::vector<std::pair<int, std::string>> filenames;

    // gather all files to be read
    dr = opendir(path);
    if (dr)
    {
        while ((it = readdir(dr)) != NULL)
        {
            filenames.push_back(
                std::pair<int, std::string>(strlen(it->d_name), it->d_name));
        }
        // sort it in log file generation order
        std::sort(filenames.begin(), filenames.end());
    }
    else
        printf("Error: %s not found\n", path);

    for (auto &i : filenames)
    {
        if (i.second != "." && i.second != "..")
            files.push_back(std::string(path) + "/" + i.second);
    }
}

/*
void removeDir(const char *path)
{
    struct stat buffer;
    if (stat(path, &buffer) == 0)
    {
        std::vector<std::string> files;
        listDir(path, files);

        for (auto &filep : files)
        {
            printf("Removing %s\n", filep.c_str());
            if (remove(filep.c_str()) != 0)
            {
                printf("Error: remove %s failed\n", filep.c_str());
            }
        }
        if (rmdir(path) == -1)
        {
            printf("Error: remove %s failed\n", path);
        }
    }
    printf("\n\n");
}
*/

bool existDir(const char *path)
{
    struct stat buffer;
    if (stat(path, &buffer) == -1) return false;
    return true;
}

void removeDir(const char *path)
{
    DIR *dir = opendir(path);
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        const char *name = entry->d_name;

        // Skip the "." and ".." entries
        if (name[0] == '.' &&
            (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
        {
            continue;
        }

        char *sub_path = (char *)malloc(strlen(path) + strlen(name) + 2);
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
