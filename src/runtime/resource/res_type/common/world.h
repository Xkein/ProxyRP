#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

#include <vector>

STRUCT(WorldResource)
{
    REFLECTION_STRUCT_BODY(WorldResource);

    PROPERTY()
    String Name;
    PROPERTY()
    std::vector<String> LevelUrls;
    PROPERTY()
    String DefaultLevelUrl;
};

