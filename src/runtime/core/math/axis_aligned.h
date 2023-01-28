#pragma once

#include "vector.h"

#include <Eigen/Geometry>

using AxisAlignedBox = Eigen::AlignedBox3f;
using BoundingBox = AxisAlignedBox;

class BoundingSphere
{
public:
    Vector3f Center;
    float    Radius;
};
