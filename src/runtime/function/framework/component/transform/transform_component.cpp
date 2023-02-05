#include "transform_component.h"

void TransformComponent::PostLoadResource(std::weak_ptr<GameObject> parent_object)
{
    Component::PostLoadResource(parent_object);

    DirtyFlag = true;
}
#include "core/math/angle.h"
void TransformComponent::Tick(float delta_time)
{
    static auto start_time   = std::chrono::high_resolution_clock::now();
    auto  current_time = std::chrono::high_resolution_clock::now();
    float time         = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
    SetRotation(Quaternionf(AngleAxisf(time, Vector3f::UnitZ())));
}

void TransformComponent::SetPosition(const Vector3f& translation)
{
    Transform.Position = translation;
    DirtyFlag            = true;
}

void TransformComponent::SetScale(const Vector3f& scale)
{
    Transform.Scale = scale;
    DirtyFlag         = true;
}

void TransformComponent::SetRotation(const Quaternionf& rotation)
{
    Transform.Rotation = rotation;
    DirtyFlag            = true;
}
