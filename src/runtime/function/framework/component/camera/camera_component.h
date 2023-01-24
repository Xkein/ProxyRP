#pragma once

#include "core/math/vector.h"
#include "core/reflection/reflection.h"
#include "function/framework/component/component.h"
#include "resource/res_type/components/camera.h"

enum class CameraMode : uint8_t
{
    FirstPerson,
    ThirdPerson,
    Free,
    Num
};

CLASS(CameraComponent : public Component)
{
    REFLECTION_CLASS_BODY(CameraComponent);

public:
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;

    virtual void Tick(float delta_time) override;

    Vector3f GetPosition() const
    {
        return Position;
    }
    Vector3f GetForward() const
    {
        return Forward;
    }

private:
    void TickFirstPersonCamera(float delta_time);
    void TickThirdPersonCamera(float delta_time);
    void TickFreePersonCamera(float delta_time);

public:
    CameraMode Mode {CameraMode::Num};

private:
    PROPERTY()
    Instance<CameraParameter> Parameter;

    Vector3f Position;

    Vector3f Forward {Vector3f::UnitY()};
    Vector3f Up {Vector3f::UnitZ()};
    Vector3f Left {Vector3f::UnitX()};
};
