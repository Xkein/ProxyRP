
#include "game_object.h"
#include "component.h"


inline GameObject::GameObject(GameObjectID id) : ID(id) {}

GameObject::~GameObject()
{
    for (Component* component : Components)
    {
        delete component;
    }

    Components.clear();
}

void GameObject::Tick(float delta_time)
{
    for (Component* component : Components)
    {
        component->Tick(delta_time);
    }
}

