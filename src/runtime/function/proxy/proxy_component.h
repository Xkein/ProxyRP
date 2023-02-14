#pragma once

#include "function/framework/component/component.h"
#include "core/math/transform.h"

struct ProxyObjectDesc
{
    uint64_t ClientHandle;
    uint64_t ServerHandle;

    uint32_t ActionCount;
};

CLASS(ProxyComponent : public Component)
{
    REFLECTION_CLASS_BODY(ProxyComponent);
    friend class ProxyManager;

public:
    virtual void Tick(float delta_time) override;

    void UpdateTransform(Transformf transform);

private:
    
    ProxyObjectDesc Desc;
};
