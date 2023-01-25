#pragma once

#include "core/math/vector.h"

struct AmbientLight
{
    Vector3f Irradiance;
};

struct DirectionalLight
{
    Vector3f Direction;
    Vector3f Color;
};

struct PointLight
{
    Vector3f Position;
    Vector3f Flux;
};
