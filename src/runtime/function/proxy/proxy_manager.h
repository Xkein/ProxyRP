#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"
#include "proxy_rpc.h"
#include "proxy_action.h"
#include "proxy_object.h"

class GameObject;

struct ProxyObjectUpdateDesc
{
    enum UpdateType
    {
        Transform,
    };

    UpdateType Type;
    union
    {
        uint64_t ServerHandle;

        ProxyTransformDesc TransformDesc;
    };
};

struct ProxyActionDesc
{
    union
    {
        ProxyObjectCreateDesc  CreateDesc;
        ProxyObjectDestroyDesc DestroyDesc;
        ProxyTransformDesc     TransformDesc;
    };
};

struct ProxyManagerInitInfo
{
    String   Address;
    uint16_t Port;
};

class ProxyManager
{
public:
    ~ProxyManager();
    void Initialize(ProxyManagerInitInfo init_info = {});
    void Clear();
    void Tick(float delta_time);

    void AddAction(StringView action, const ProxyActionDesc& action_desc);

private:
    void CreateProxyGameObject(const ProxyObjectCreateDesc & create_desc);
    void DestroyProxyGameObject(const ProxyObjectDestroyDesc & destroy_desc);

    void UpdateProxyGameObject(const ProxyObjectUpdateDesc & update_desc);

    void ProcessActions();

    String   Address;
    uint16_t Port;

    ProxyRPC RPC;

    std::map<int, std::shared_ptr<GameObject>> ProxyGObjects;

    std::vector<ProxyObjectCreateDesc>  CreateDescs;
    std::vector<ProxyObjectDestroyDesc> DestroyDescs;
    std::vector<ProxyObjectUpdateDesc>  UpdateDescs;
};

