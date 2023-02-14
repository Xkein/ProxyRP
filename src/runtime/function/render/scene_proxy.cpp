#include "scene_proxy.h"
#include "function/global/global_context.h"
#include "function/render/render_system.h"
#include "function/render/scene_proxy.h"
#include "function/framework/component/light/point_light_component.h"

LightSceneProxy::LightSceneProxy(const LightComponent* light_component) :
    Component(light_component), Intensity(light_component->GetIntensity()), Position(light_component->GetPosition()), Color(light_component->GetLightColor())
{}

PointLightSceneProxy::PointLightSceneProxy(const PointLightComponent* point_light_component) : LightSceneProxy(point_light_component), Radius(point_light_component->GetRadius())
{
}
