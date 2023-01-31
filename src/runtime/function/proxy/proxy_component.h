#pragma once

#include "function/framework/component/component.h"

CLASS(ProxyComponent : public Component)
{
    REFLECTION_CLASS_BODY(ProxyComponent);

public:
    virtual void Tick(float delta_time) override;

private:
    
};
