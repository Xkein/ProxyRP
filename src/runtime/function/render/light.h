#pragma once

#include "core/math/vector.h"

struct AmbientLight
{
    glm::vec3 Irradiance;
};

struct DirectionalLight
{
    glm::vec3 Direction;
    glm::vec3 Color;
};

struct PointLight
{
    glm::vec3 Position;
    glm::vec3 Flux;
};
