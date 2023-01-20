#pragma once

#include "core/reflection/reflection.h"
#include "core/math/transform.h"
#include "function/framework/component/component.h"

CLASS(TransformComponent : public Component)
{
    REFLECTION_CLASS_BODY(TransformComponent);

public:
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;

protected:
    PROPERTY()
    Transformf Transform;
};
