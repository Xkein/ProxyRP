#include "file_manager.h"
#include "core/log/log_system.h"

#include <fstream>
#include <sstream>

std::vector<byte> FileManager::Read(const Char* path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        LOG_ERROR("failed to open {}", path);
    }

    size_t            fileSize = (size_t)file.tellg();
    std::vector<byte> buffer(fileSize);

    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);

    file.close();

    return buffer;
}

String FileManager::ReadString(const Char* path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        LOG_ERROR("failed to open {}", path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    String text(buffer.str());

    return text;
}

bool FileManager::Write(const Char* path, byte* data, size_t count)
{
    std::ofstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        LOG_ERROR("failed to write {}", path);
        return false;
    }

    file.write((char*)data, count);

    file.flush();
    file.close();

    return true;
}

bool FileManager::WriteString(const Char* path, const Char* str)
{
    std::ofstream file(path);
    if (!file)
    {
        LOG_ERROR("failed to write {}", path);
        return false;
    }

    file << str;
    file.flush();
    file.close();

    return true;
}
