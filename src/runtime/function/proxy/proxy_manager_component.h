#pragma once

#include "core/reflection/reflection.h"
#include "function/framework/component/component.h"

class ProxyManager;

CLASS(ProxyManagerComponent : public Component)
{
    REFLECTION_CLASS_BODY(ProxyManagerComponent);

public:
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;

    virtual void Tick(float delta_time) override;

    
    PROPERTY()
    String Address {"localhost"};
    PROPERTY()
    uint16_t Port {0};

    bool                          Initialized {false};
    std::shared_ptr<ProxyManager> Manager;
};

