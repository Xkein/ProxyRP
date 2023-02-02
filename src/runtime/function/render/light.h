#pragma once

#include "core/math/math_const.h"
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

    
    // calculate an appropriate radius for light culling
    // a windowing function in the shader will perform a smooth transition to zero
    // this is not physically based and usually artist controlled
    float CalculateRadius() const
    {
        // radius = where attenuation would lead to an intensity of 1W/m^2
        const float INTENSITY_CUTOFF    = 1.0f;
        const float ATTENTUATION_CUTOFF = 0.05f;
        Vector3f    intensity           = GetIntensity();
        float       maxIntensity        = intensity.maxCoeff();
        float       attenuation         = std::max(INTENSITY_CUTOFF, ATTENTUATION_CUTOFF * maxIntensity) / maxIntensity;
        return 1.0f / sqrtf(attenuation);
    }

    Vector3f GetIntensity() const {
        return Flux / (4.0f * Math_PI);
    }
};
