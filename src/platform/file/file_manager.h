#pragma once

#include <vector>

#include "platform/platform.h"
#include "platform/string.h"

class FileManager
{
public:
    static std::vector<byte> Read(const Char* path);
};
