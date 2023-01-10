#pragma once

#include "core/reflection.h"

#include <memory>
#include <vector>

class Component;

using GameObjectID = size_t;

CLASS()
class GameObject : std::enable_shared_from_this<GameObject>
{
    REFLECTION_CLASS_BODY()
public:
    GameObject(GameObjectID id);
    ~GameObject();

    void Tick(float delta_time);

    GameObjectID GetID() const { return ID; }

private:
    GameObjectID ID;
    PROPERTY()
    std::vector<Component*> Components;
};