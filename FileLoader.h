#ifndef DECOMPILER_OBJECTLOADER_H
#define DECOMPILER_OBJECTLOADER_H

#include <filesystem>
#include <fstream>
#include <ios>
#include <span>
#include <vector>

class FileLoader {
public:
    explicit FileLoader(const std::filesystem::path &path) {
        std::ifstream file{path, std::ios::binary | std::ios::ate};

        if (!file) {
            throw std::runtime_error(
                "The file does not exist: " + path.string());
        }

        const auto size = file.tellg();
        data.resize(size);

        if (!file.read(data.data(), size)) {
            throw std::runtime_error("Unable to read file: " + path.string());
        }
    }

    std::span<const char> getData() {
        return data;
    }

private:
    std::vector<char> data{};
};

#endif //DECOMPILER_OBJECTLOADER_H
