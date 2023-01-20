#pragma once

#include "core/reflection/reflection.h"
#include "function/framework/component/component.h"
#include "function/proxy/proxy_manager.h"

CLASS(ProxyManagerComponent : public Component)
{
    REFLECTION_CLASS_BODY(ProxyManagerComponent);

public:
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;

    PROPERTY()
    ProxyManager Manager;
};