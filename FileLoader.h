#ifndef DECOMPILER_OBJECTLOADER_H
#define DECOMPILER_OBJECTLOADER_H

#include <string>
#include <span>
#include <cstddef>
#include <stdexcept>
#include <filesystem>
#include <optional>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class FileLoader {
public:
    explicit FileLoader(const std::filesystem::path &path) {
        load(path);
    }

    ~FileLoader() {
        unload();
    }

    std::span<const char> getData() {
        if (!data) return {};
        return {static_cast<const char*>(data), size};
    }

private:
    int fd{-1};
    void *data{nullptr};
    size_t size{};

    void load(const std::filesystem::path &path) {
        fd = open(path.c_str(), O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error("Failed to open the file: " + path.string());
        }

        struct stat sb{};
        if (fstat(fd, &sb) == -1) {
            close(fd);
            throw std::runtime_error("Failed to get the size of: " + path.string());
        }

        size = static_cast<size_t>(sb.st_size);
        if (size == 0) {
            throw std::runtime_error("The size of the file is 0: " + path.string());
        }

        data = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (data == nullptr || data == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Failed to mmap the file: " + path.string());
        }
    }

    void unload() const {
        if (data != nullptr && data != MAP_FAILED)
            munmap(data, size);
        if (fd != -1)
            close(fd);
    }
};

#endif //DECOMPILER_OBJECTLOADER_H
