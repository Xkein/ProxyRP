#include "transform_component.h"
#include "function/framework/object/game_object.h"
#include "function/framework/component/light/point_light_component.h"

void TransformComponent::PostLoadResource(std::weak_ptr<GameObject> parent_object)
{
    Component::PostLoadResource(parent_object);

    DirtyFlag = true;
}

void TransformComponent::Tick(float delta_time)
{
}

void TransformComponent::SetDirtyFlag(bool is_dirty)
{
    DirtyFlag = is_dirty;

    std::shared_ptr<GameObject> parent_object = ParentObject.lock();

    if (PointLightComponent* point_light_component = parent_object->GetComponent<PointLightComponent>())
    {
        point_light_component->SetDirtyFlag(is_dirty);
    }
}

void TransformComponent::SetPosition(const Vector3f& translation)
{
    Transform.Position = translation;
    SetDirtyFlag(true);
}

void TransformComponent::SetScale(const Vector3f& scale)
{
    Transform.Scale = scale;
    SetDirtyFlag(true);
}

void TransformComponent::SetRotation(const Quaternionf& rotation)
{
    Transform.Rotation = rotation;
    SetDirtyFlag(true);
}

Vector3f TransformComponent::GetPosition() const
{
    return Transform.Position;
}

Vector3f TransformComponent::GetScale() const
{
    return Transform.Scale;
}

Quaternionf TransformComponent::GetRotation() const
{
    return Transform.Rotation;
}

void TransformComponent::SetTransform(const Transformf& transform)
{
    Transform = transform;
    SetDirtyFlag(true);
}

const Transformf& TransformComponent::GetTransform() const
{
    return Transform;
}

Matrix4x4 TransformComponent::GetMatrix() const
{
    return Transform.GetMatrix();
}
