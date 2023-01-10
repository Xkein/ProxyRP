#pragma once

#include "core/math.h"
#include "resource/texture/texture_resource.h"

#include <vector>

struct RawVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct MeshResource
{
    std::vector<RawVertex>          Vertices;
    std::vector<uint32_t>        Indices;
    std::vector<TextureResource> Textures;
};
