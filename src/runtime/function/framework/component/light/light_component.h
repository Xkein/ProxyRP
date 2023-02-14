#pragma once

#include "function/framework/component/component.h"
#include "function/render/light.h"

class LightSceneProxy;

CLASS(LightComponent : public Component)
{
    REFLECTION_CLASS_BODY(LightComponent);

    friend class RenderScene;

public:
    LightComponent() {}
    virtual ~LightComponent() {}

    virtual LightSceneProxy* CreateSceneProxy() const = 0;

    ELightType GetLightType() const;

    Vector3f GetPosition() const;

    Vector3f GetLightColor() const;
    void     SetLightColor(Vector3f color);

    float GetIntensity() const;
    void  SetIntensity(float intensity);

    bool IsDirty() const;
    void SetDirtyFlag(bool is_dirty);

protected:
    LightSceneProxy* SceneProxy;

    ELightType LightType {ELightType::Invalid};
    bool DirtyFlag {false};

    PROPERTY()
    Vector3f Color;
    PROPERTY()
    float Intensity;

};
