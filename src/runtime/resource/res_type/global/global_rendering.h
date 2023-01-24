#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

ENUM_CLASS(ERenderPipelineType : uint8_t)
{
    Forward  META(DisplayName = "Forward Rendering Pipeline"),
    Deferred META(DisplayName = "Deferred Rendering Pipeline"),
    Num,
};

STRUCT(CameraPose)
{
    REFLECTION_STRUCT_BODY(CameraPose);

    PROPERTY()
    Vector3f Position;
    PROPERTY()
    Vector3f Target;
    PROPERTY()
    Vector3f Up;
};

STRUCT(CameraConfig)
{
    REFLECTION_STRUCT_BODY(CameraConfig);

    PROPERTY()
    CameraPose Pose;
    PROPERTY()
    Vector2f Aspect;
    PROPERTY()
    float ZFar;
    PROPERTY()
    float ZNear;
};

STRUCT(DirectionalLightResource)
{
    REFLECTION_STRUCT_BODY(DirectionalLightResource);

    PROPERTY()
    Vector3f Direction;
    PROPERTY()
    Vector3f Color;
};

STRUCT(GlobalRenderingResource)
{
    REFLECTION_STRUCT_BODY(GlobalRenderingResource);

    PROPERTY()
    CameraConfig CameraConfig;

    PROPERTY()
    ERenderPipelineType RenderPipelineType;

    PROPERTY()
    DirectionalLightResource DirectionalLight;
};
