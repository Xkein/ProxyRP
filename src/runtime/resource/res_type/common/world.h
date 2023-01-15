#pragma once

#include "platform/string.h"

#include <vector>

struct WorldResource
{
    String Name;

    std::vector<String> LevelUrls;

    String DefaultLevelUrl;
};

