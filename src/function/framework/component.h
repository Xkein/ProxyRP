#pragma once

#include "core/reflection.h"

#include <memory>

class GameObject;

CLASS()
class Component
{
    REFLECTION_CLASS_BODY()
public:
    Component() = default;
    virtual ~Component() {}

    virtual void Tick(float delta_time) {}

protected:
    std::weak_ptr<GameObject> m_game_object;
};