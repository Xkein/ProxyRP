#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

STRUCT(MaterialResource)
{
    REFLECTION_STRUCT_BODY(MaterialResource);

    PROPERTY()
    String BaseColorTexture;
    PROPERTY()
    String NormalTexture;
};

