#pragma once

#include "core/reflection/reflection.h"
#include "function/framework/component/component.h"

CLASS(CameraComponent : public Component)
{
    REFLECTION_CLASS_BODY(CameraComponent);

public:
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;
};
