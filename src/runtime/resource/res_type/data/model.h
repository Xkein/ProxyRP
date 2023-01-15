#pragma once

#include <memory>

#include "resource/res_type/data/mesh.h"
#include "platform/platform.h"
#include "platform/string.h"

struct Model
{
    String              Path;
    std::vector<MeshResource> Meshes;
};

