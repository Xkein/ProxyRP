#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

STRUCT(GlobalRenderingResource)
{
    REFLECTION_STRUCT_BODY(GlobalRenderingResource);

    PROPERTY()
    bool EnableFxaa {false};
};
