#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

STRUCT(MaterialResource)
{
    REFLECTION_STRUCT_BODY(MaterialResource);

    PROPERTY()
    String BaseColorFile;
    PROPERTY()
    String NormalFile;
    PROPERTY()
    String MetallicRoughnessFile;
    PROPERTY()
    String OcclusionFile;
    PROPERTY()
    String EmissiveFile;
};

