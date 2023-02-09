#pragma once

#include "core/math/math.h"

#include <memory>

class LightComponent;
class PointLightComponent;

class PrimitiveSceneProxy
{
public:
};

class LightSceneProxy
{
public:
    LightSceneProxy(const LightComponent* light_component);

public:
    const LightComponent* Component;

    int LightType;

    float Intensity;

    Vector3f Position;

    Vector3f Color;

    Matrix4x4 WorldToLight;

    Matrix4x4 LightToWorld;
};

class PointLightSceneProxy : public LightSceneProxy
{
public:
    PointLightSceneProxy(const PointLightComponent* point_light_component);

    float Radius;
};
