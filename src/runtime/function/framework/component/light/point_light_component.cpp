#include "point_light_component.h"
#include "function/global/global_context.h"
#include "function/render/render_system.h"
#include "function/render/render_scene.h"
#include "function/render/scene_proxy.h"

PointLightComponent::PointLightComponent()
{
    LightType = ELightType::Point;
}

PointLightComponent::~PointLightComponent()
{
    GRenderSystem->GetScene()->RemoveLight(this);
}

void PointLightComponent::PostLoadResource(std::weak_ptr<GameObject> parent_object)
{
    Component::PostLoadResource(parent_object);

    DirtyFlag = true;
}

void PointLightComponent::Tick(float delta_time)
{
    if (!IsDirty())
        return;
    if (!_Registered)
    {
        GRenderSystem->GetScene()->AddLight(this);
        _Registered = true;
    }

    GRenderSystem->GetScene()->UpdateLight(this);
}

LightSceneProxy* PointLightComponent::CreateSceneProxy() const
{
    return new PointLightSceneProxy(this);
}

float PointLightComponent::GetRadius() const
{
    return Radius;
}
void PointLightComponent::SetRadius(float radius)
{
    Radius = radius;
    DirtyFlag = true;
}
