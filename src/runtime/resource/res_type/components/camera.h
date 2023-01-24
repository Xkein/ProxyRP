#pragma once

#include "core/math/quaternion.h"
#include "core/reflection/reflection.h"

STRUCT(CameraParameter)
{
    PROPERTY()
    float Fov {50.f};
};

STRUCT(FirstPersonCameraParameter : public CameraParameter)
{
    PROPERTY()
    float VerticalOffset {0.6f};
};

STRUCT(ThirdPersonCameraParameter : public CameraParameter)
{
    PROPERTY()
    float HorizontalOffset {3.f};
    PROPERTY()
    float VerticalOffset {2.5f};

    Quaternionf CursorPitch;
    Quaternionf CursorYaw;
};

STRUCT(FreeCameraParameter : public CameraParameter)
{
    PROPERTY()
    float Speed {0.6f};
};
