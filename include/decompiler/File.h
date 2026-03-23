#pragma once

#include <sys/stat.h>

struct File {
    int fd;
    char *data;
    struct stat sb;

    explicit File(char *path);

    ~File();
};
