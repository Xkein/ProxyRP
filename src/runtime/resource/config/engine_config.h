#pragma once

#include <filesystem>
#include "core/macro.h"

class EngineConfig
{
public:
    void Load(const std::filesystem::path& config_path);

public:
    bool VSync;

};

