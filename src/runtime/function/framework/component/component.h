#pragma once

#include "core/reflection/reflection.h"

#include <memory>

class GameObject;

CLASS()
class Component
{
    REFLECTION_CLASS_BODY(Component)
public:
    Component() = default;
    virtual ~Component() {}

    // Instantiating the component after definition loaded
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object);

    virtual void Tick(float delta_time) {}

protected:
    std::weak_ptr<GameObject> ParentObject;
};

