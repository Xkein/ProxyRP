#include "angle.h"

float DegreesToRadians(float degrees)
{
    return degrees * Math_fDeg2Rad;
}

float RadiansToDegrees(float radians)
{
    return radians * Math_fRad2Deg;
}
