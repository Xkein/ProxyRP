#pragma once

#include "function/framework/component/component.h"

CLASS(AnimationComponent : public Component)
{
    REFLECTION_CLASS_BODY(AnimationComponent);

public:
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;

    virtual void Tick(float delta_time) override;

private:

};