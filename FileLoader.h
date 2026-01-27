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
        std::ifstream file{path, std::ios::binary};
        file.unsetf(std::ios::skipws);

        if (!file) {
            throw std::runtime_error("The file from path does not exist");
        }

        const auto size = std::filesystem::file_size(path);
        data.resize(size);

        file.read(data.data(), size);
    }

    std::span<char> getData() {
        return data;
    }

private:
    std::vector<char> data{};
};

#endif //DECOMPILER_OBJECTLOADER_H
