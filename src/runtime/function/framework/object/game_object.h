#pragma once

#include "object_id_allocator.h"
#include "core/reflection/reflection.h"
#include "platform/string.h"

#include <memory>
#include <vector>

class Component;
class ObjectInstanceResource;

class GameObject : std::enable_shared_from_this<GameObject>
{
public:
    GameObject(GameObjectID id);
    ~GameObject();

    void Tick(float delta_time);

    bool Load(const ObjectInstanceResource& object_instance_res);

    bool HasComponent(const rttr::type& type);

    GameObjectID GetID() const { return ID; }

    void          SetName(const String& name) { Name = name; }
    const String& GetName() const { return Name; }

private:
    void Clear();

    String Name;
    String DefinitionUrl;

    GameObjectID ID;
    std::vector<Instance<Component>> Components;
};