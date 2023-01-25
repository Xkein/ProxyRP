#pragma once

#include "core/reflection/reflection.h"

#include <memory>

class GameObject;

CLASS(Component)
{
    REFLECTION_CLASS_BODY(Component)
public:
    Component() = default;
    virtual ~Component() {}

    /**
     *  Instantiating the component after definition loaded.
     * 
     * \param parent_object
     */
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object)
    {
        ParentObject = parent_object;
    }

    virtual void Tick(float delta_time) {}

protected:
    std::weak_ptr<GameObject> ParentObject;
};

