#pragma once

#include <vector>

#include "platform/platform.h"
#include "platform/string.h"

class FileManager
{
public:
    static std::vector<byte> Read(const Char* path);
    static String            ReadString(const Char* path);

    static bool Write(const Char* path, byte* data, size_t count);
    static bool WriteString(const Char* path, const Char* str);



    static std::vector<byte> Read(StringView path)
    {
        return Read(path.data());
    }
    static String ReadString(StringView path)
    {
        return ReadString(path.data());
    }

    static bool Write(StringView path, std::vector<byte> data)
    {
        return Write(path.data(), data.data(), data.size());
    }
    static bool WriteString(StringView path, StringView str)
    {
        return WriteString(path.data(), str.data());
    }
};
