#pragma once

#include "core/reflection/reflection.h"
#include "core/math/transform.h"
#include "core/math/vector.h"
#include "core/math/quaternion.h"
#include "function/framework/component/component.h"

CLASS(TransformComponent : public Component)
{
    REFLECTION_CLASS_BODY(TransformComponent);

public:
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;

    virtual void Tick(float delta_time) override;

    void SetPosition(const Vector3f& translation);
    void SetScale(const Vector3f& scale);
    void SetRotation(const Quaternionf& rotation);

    Transformf& GetTransform()
    {
        return Transform;
    }
    const Transformf& GetTransform() const
    {
        return Transform;
    }

protected:
    PROPERTY()
    Transformf Transform;

    bool IsDirty {false};
};
