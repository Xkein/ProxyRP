#include "file_manager.h"

#include <fstream>

std::vector<byte> FileManager::Read(const Char* path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t            fileSize = (size_t)file.tellg();
    std::vector<byte> buffer(fileSize);

    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);

    file.close();

    return buffer;
}
