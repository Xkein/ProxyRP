#pragma once

#include <memory>

#include "resource/mesh/mesh_resource.h"
#include "platform/platform.h"
#include "platform/string.h"

struct Model
{
    String              Path;
    std::vector<MeshResource> Meshes;
};

