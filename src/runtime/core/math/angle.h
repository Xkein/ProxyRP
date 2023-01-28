#pragma once

#include "math_const.h"

#include <Eigen/Geometry>

float DegreesToRadians(float degrees)
{
    return degrees * Math_fDeg2Rad;
}

float RadiansToDegrees(float radians)
{
    return radians * Math_fRad2Deg;
}

using AngleAxisf = Eigen::AngleAxisf;

