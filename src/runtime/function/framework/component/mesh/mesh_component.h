#pragma once

#include "core/reflection/reflection.h"
#include "function/framework/component/component.h"

CLASS(MeshComponent : public Component)
{
    REFLECTION_CLASS_BODY(MeshComponent);

public:
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;

};