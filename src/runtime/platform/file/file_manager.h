#pragma once

#include <vector>

#include "platform/platform.h"
#include "platform/string.h"

class FileManager
{
public:
    static std::vector<byte> Read(const Char* path);
    static String            ReadString(const Char* path);

    static std::vector<byte> Read(const String& path)
    {
        return Read(path.c_str());
    }
    static String ReadString(const String& path)
    {
        return ReadString(path.c_str());
    }

    static bool WriteString(const Char* path, const Char* str);
    static bool WriteString(const String& path, const String& str)
    {
        return WriteString(path.c_str(), str.c_str());
    }
};
