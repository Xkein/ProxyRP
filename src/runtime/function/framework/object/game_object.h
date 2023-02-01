#pragma once

#include "object_id_allocator.h"
#include "core/reflection/reflection.h"
#include "platform/string.h"

#include <memory>
#include <vector>

class Component;
class ObjectInstanceResource;

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    GameObject(GameObjectID id);
    ~GameObject();

    void Tick(float delta_time);

    bool Load(const ObjectInstanceResource& object_instance_res);

    GameObjectID GetID() const
    {
        return ID;
    }

    void SetName(const String& name)
    {
        Name = name;
    }
    const String& GetName() const
    {
        return Name;
    }

    bool HasComponent(const rttr::type& type) const;

    Component* GetComponent(const rttr::type& type);

    template<typename TComponent>
    TComponent* GetComponent()
    {
        return static_cast<TComponent*>(GetComponent(rttr::type::get<TComponent>()));
    }

    std::vector<Instance<Component>> GetComponents() {
        return Components;
    }

    template<typename TComponent>
    TComponent* AddComponent()
    {
        Instance<Component> instance(rttr::type::get<TComponent>(), new TComponent());
        Components.emplace_back(std::move(instance));
    }

    void RemoveComponent(const rttr::type& type);
    template<typename TComponent>
    void RemoveComponent()
    {
        RemoveComponent(rttr::type::get<TComponent>());
    }


private:
    void Clear();

    String Name;
    String DefinitionUrl;

    GameObjectID ID;
    std::vector<Instance<Component>> Components;
};