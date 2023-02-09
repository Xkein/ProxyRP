#include "light_component.h"
#include "function/framework/component/transform/transform_component.h"
#include "function/framework/object/game_object.h"

ELightType LightComponent::GetLightType() const
{
    return (ELightType)LightType;
}

Vector3f LightComponent::GetPosition() const
{
    std::shared_ptr<GameObject> parent_object = ParentObject.lock();
    TransformComponent* transform_component = parent_object->GetComponent<TransformComponent>();

    return transform_component->GetTransform().Position;
}

Vector3f LightComponent::GetLightColor() const
{
    return Color;
}
void LightComponent::SetLightColor(Vector3f color)
{
    Color     = color;
    DirtyFlag = true;
}

float LightComponent::GetIntensity() const
{
    return Intensity;
}
void LightComponent::SetIntensity(float intensity)
{
    Intensity = intensity;
    DirtyFlag = true;
}

bool LightComponent::IsDirty() const
{
    return DirtyFlag;
}
void LightComponent::SetDirtyFlag(bool is_dirty)
{
    DirtyFlag = is_dirty;
}
