#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

STRUCT(MeshResource)
{
    REFLECTION_STRUCT_BODY(MeshResource);

    PROPERTY()
    String MeshFile;
};
