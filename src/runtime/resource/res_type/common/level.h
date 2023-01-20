#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"
#include "resource/res_type/common/object.h"

#include <vector>

STRUCT(LevelResource)
{
    REFLECTION_STRUCT_BODY(LevelResource);

    PROPERTY()
    std::vector<ObjectInstanceResource> Objects;
};
