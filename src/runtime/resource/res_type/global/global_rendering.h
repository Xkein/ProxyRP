#pragma once

#include "core/math/vector.h"
#include "core/reflection/reflection.h"
#include "platform/string.h"

ENUM_CLASS(ERenderPipelineType : uint8_t)
{
    Forward  META(DisplayName = "Forward Rendering Pipeline"),
    Deferred META(DisplayName = "Deferred Rendering Pipeline"),
    Num,
};

STRUCT(SkyBoxIrradianceMap)
{
    REFLECTION_STRUCT_BODY(SkyBoxIrradianceMap);

    PROPERTY()
    String m_negative_x_map;
    PROPERTY()
    String m_positive_x_map;
    PROPERTY()
    String m_negative_y_map;
    PROPERTY()
    String m_positive_y_map;
    PROPERTY()
    String m_negative_z_map;
    PROPERTY()
    String m_positive_z_map;
};

STRUCT(SkyBoxSpecularMap)
{
    REFLECTION_STRUCT_BODY(SkyBoxSpecularMap);

    PROPERTY()
    String m_negative_x_map;
    PROPERTY()
    String m_positive_x_map;
    PROPERTY()
    String m_negative_y_map;
    PROPERTY()
    String m_positive_y_map;
    PROPERTY()
    String m_negative_z_map;
    PROPERTY()
    String m_positive_z_map;
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
    ERenderPipelineType RenderPipelineType;

    PROPERTY()
    SkyBoxIrradianceMap SkyboxIrradianceMap;
    PROPERTY()
    SkyBoxSpecularMap SkyboxSpecularMap;
    PROPERTY()
    String BrdfMap;
    PROPERTY()
    String ColorGradingMap;
    PROPERTY()
    Vector3f SkyColor;
    PROPERTY()
    Vector3f AmbientLight;
    PROPERTY()
    DirectionalLightResource DirectionalLight;

    PROPERTY()
    CameraConfig CameraConfig;


};
