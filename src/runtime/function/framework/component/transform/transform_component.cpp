#include "transform_component.h"

void TransformComponent::PostLoadResource(std::weak_ptr<GameObject> parent_object)
{
    Component::PostLoadResource(parent_object);

    IsDirty = true;
}

void TransformComponent::Tick(float delta_time)
{

}

void TransformComponent::SetPosition(const Vector3f& translation)
{
    Transform.Position = translation;
    IsDirty            = true;
}

void TransformComponent::SetScale(const Vector3f& scale)
{
    Transform.Scale = scale;
    IsDirty         = true;
}

void TransformComponent::SetRotation(const Quaternionf& rotation)
{
    Transform.Rotation = rotation;
    IsDirty            = true;
}
