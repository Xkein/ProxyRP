#include "proxy_manager.h"
#include "proxy_component.h"
#include "function/global/global_context.h"
#include "function/framework/world/world_manager.h"
#include "resource/res_type/common/object.h"
#include <mutex>
std::mutex proxy_manager_mutex;

ProxyManager::~ProxyManager()
{
    Clear();
}

void ProxyManager::Initialize(ProxyManagerInitInfo init_info)
{
    Address = init_info.Address;
    Port    = init_info.Port;

    ProxyRPCInitInfo rpc_init_info;
    rpc_init_info.Address = Address != "localhost" ? Address.c_str() : "127.0.0.1";
    rpc_init_info.Port    = Port;
    rpc_init_info.Manager = this;
    RPC.Initialize(rpc_init_info);
}

void ProxyManager::Clear()
{
    RPC.Clear();

    std::shared_ptr<Level> level = GWorldManager->GetCurrentLevel().lock();

    for (auto& [_, game_object] : ProxyGObjects)
    {
        level->DeleteGObjectByID(game_object->GetID());
    }

    ProxyGObjects.clear();
}

void ProxyManager::Tick(float delta_time)
{
    std::lock_guard<std::mutex> guard(proxy_manager_mutex);

    ProcessActions();
}

void ProxyManager::CreateProxyGameObject(const ProxyObjectCreateDesc& create_desc)
{
    std::shared_ptr<Level> level = GWorldManager->GetCurrentLevel().lock();

    std::shared_ptr<GameObject> game_object;
    uint64_t                    server_handle = HandleMap[create_desc.ClientHandle];

    ObjectInstanceResource object_instance_res;
    object_instance_res.Name          = create_desc.ObjectName;
    object_instance_res.DefinitionUrl = create_desc.ObjectDefinitionUrl;
    level->CreateGObject(object_instance_res, &game_object);

    ProxyComponent* proxy_component = game_object->AddComponent<ProxyComponent>();

    proxy_component->Desc.ClientHandle = create_desc.ClientHandle;
    proxy_component->Desc.ServerHandle = server_handle;
    proxy_component->Desc.ActionCount  = 0;

    ProxyGObjects[server_handle] = game_object;
}

void ProxyManager::DestroyProxyGameObject(const ProxyObjectDestroyDesc& destroy_desc)
{
    auto iter = ProxyGObjects.find(destroy_desc.ServerHandle);
    if (iter == ProxyGObjects.end())
        return;

    std::shared_ptr<Level> level = GWorldManager->GetCurrentLevel().lock();

    std::shared_ptr<GameObject> game_object     = iter->second;
    ProxyComponent*             proxy_component = game_object->GetComponent<ProxyComponent>();
    if (proxy_component)
    {
        HandleMap.erase(proxy_component->Desc.ClientHandle);
    }

    level->DeleteGObjectByID(game_object->GetID());

    ProxyGObjects.erase(iter);
}


void ProxyManager::UpdateProxyGameObject(const ProxyObjectUpdateDesc& update_desc)
{
    auto iter = ProxyGObjects.find(update_desc.ServerHandle);
    if (iter == ProxyGObjects.end())
        return;

    std::shared_ptr<GameObject> game_object     = iter->second;
    ProxyComponent*             proxy_component = game_object->GetComponent<ProxyComponent>();

    if (!proxy_component)
        return;

    if (update_desc.Type == ProxyObjectUpdateDesc::UpdateType::Transform)
    {
        Transformf transform;
        transform.Position = Vector3f(update_desc.TransformDesc.Position);
        transform.Scale    = Vector3f(update_desc.TransformDesc.Scale);
        transform.Rotation = Quaternionf(update_desc.TransformDesc.Quaternion);
        proxy_component->UpdateTransform(transform);
    }
}

void ProxyManager::AddAction(StringView action, const ProxyActionDesc& action_desc, ProxyActionResponse& response)
{
    std::lock_guard<std::mutex> guard(proxy_manager_mutex);

    if (action == PA_CREATE_OBJECT)
    {
        if (HandleMap.contains(action_desc.CreateDesc.ClientHandle))
        {
            response.ServerHandle = HandleMap[action_desc.CreateDesc.ClientHandle];
            return;
        }
        CreateDescs.push_back(action_desc.CreateDesc);
        HandleMap[action_desc.CreateDesc.ClientHandle] = response.ServerHandle = action_desc.CreateDesc.ClientHandle;
    }
    else if (action == PA_DESTROY_OBJECT)
    {
        DestroyDescs.push_back(action_desc.DestroyDesc);
    }
    
    // update object

    else if (action == PA_UPDATE_OBJECT_TRANSFORM)
    {
        ProxyObjectUpdateDesc desc;
        desc.Type = ProxyObjectUpdateDesc::UpdateType::Transform;
        desc.TransformDesc = action_desc.TransformDesc;
        UpdateDescs.push_back(desc);
    }

}

void ProxyManager::ProcessActions()
{
    for (const ProxyObjectDestroyDesc& desc : DestroyDescs)
    {
        DestroyProxyGameObject(desc);
    }
    DestroyDescs.clear();

    for (const ProxyObjectCreateDesc& desc : CreateDescs)
    {
        CreateProxyGameObject(desc);
    }
    CreateDescs.clear();

    for (const ProxyObjectUpdateDesc& desc : UpdateDescs)
    {
        UpdateProxyGameObject(desc);
    }
    UpdateDescs.clear();
}
