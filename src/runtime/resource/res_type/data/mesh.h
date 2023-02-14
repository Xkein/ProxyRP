#pragma once

#include "core/math/transform.h"
#include "core/reflection/reflection.h"
#include "platform/string.h"

STRUCT(SubMeshResource)
{
    REFLECTION_STRUCT_BODY(SubMeshResource);

    PROPERTY()
    String MeshFileRef;
    PROPERTY()
    Transformf Transform;
    PROPERTY()
    String Material;
};

STRUCT(MeshComponentResource)
{
    REFLECTION_STRUCT_BODY(MeshComponentResource);

    PROPERTY()
    std::vector<SubMeshResource> SubMeshes;
};