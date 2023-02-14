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

    Vector3f    GetPosition() const;
    Vector3f    GetScale() const;
    Quaternionf GetRotation() const;

    void              SetTransform(const Transformf& transform);
    const Transformf& GetTransform() const;

    Matrix4x4 GetMatrix() const;

    bool IsDirty() const {
        return DirtyFlag;
    }
    void SetDirtyFlag(bool is_dirty);

private:
    PROPERTY()
    Transformf Transform;

    bool DirtyFlag {false};
};
