
#include "game_object.h"
#include "function/framework/component/component.h"
#include "function/global/global_context.h"
#include "resource/asset/asset_manager.h"
#include "resource/res_type/common/object.h"


GameObject::GameObject(GameObjectID id) : ID(id) {}

GameObject::~GameObject()
{
    Clear();
}

void GameObject::Tick(float delta_time)
{
    for (auto& component : Components)
    {
        component->Tick(delta_time);
    }
}

bool GameObject::Load(const ObjectInstanceResource& object_instance_res)
{
    Clear();

    SetName(object_instance_res.Name);

    Components = object_instance_res.InstancedComponents;
    for (auto& component : Components)
    {
        if (component)
        {
            component->PostLoadResource(weak_from_this());
        }
    }

    DefinitionUrl = object_instance_res.DefinitionUrl;

    ObjectDefinitionResource definition_res;
    if (const bool is_loaded_success = GAssetManager->LoadAsset(DefinitionUrl, definition_res); !is_loaded_success)
    {
        return false;
    }

    for (auto& loaded_component : definition_res.Components)
    {
        if (HasComponent(loaded_component.GetType()))
            continue;

        loaded_component->PostLoadResource(weak_from_this());

        Components.push_back(loaded_component);
    }

    return true;
}

bool GameObject::HasComponent(const rttr::type& type) const
{
    for (const auto& component : Components)
    {
        if (component.GetType() == type)
            return true;
    }

    return false;
}

Component* GameObject::GetComponent(const rttr::type& type)
{
    for (auto& component : Components)
    {
        if (component.GetType() == type)
            return component.GetPtr();
    }

    return nullptr;
}

void GameObject::Clear()
{
    for (auto& component : Components)
    {
        REFLECTION_DELETE(component);
    }

    Components.clear();
}

