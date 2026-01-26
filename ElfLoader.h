#ifndef DECOMPILER_ELFLOADER_H
#define DECOMPILER_ELFLOADER_H

#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

class ElfLoader {
public:
    explicit ElfLoader(const std::string &path) {
        std::ifstream file{path, std::ios::binary};
        file.unsetf(std::ios::skipws);

        if (!file) {
            std::cerr << "Error opening file: " << path;
            exit(1);
        }

        const auto size = std::filesystem::file_size(path);
        data.resize(size);

        file.read(data.data(), size);
    }

    [[nodiscard]] const std::vector<char> &getData() const { return data; }

private:
    std::vector<char> data{};
};


#endif //DECOMPILER_ELFLOADER_H
