#pragma once

#include "light_component.h"

CLASS(PointLightComponent : public LightComponent)
{
    REFLECTION_CLASS_BODY(PointLightComponent);

public:
    PointLightComponent();
    virtual ~PointLightComponent();

    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;

    virtual void Tick(float delta_time) override;

    virtual LightSceneProxy* CreateSceneProxy() const override;

    float GetRadius() const;
    void  SetRadius(float radius);

private:
    PROPERTY()
    float Radius;

    bool _Registered {false};
};