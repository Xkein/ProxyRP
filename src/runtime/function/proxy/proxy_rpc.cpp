#include "proxy_rpc.h"
#include "proxy_action.h"
#include "proxy_object.h"
#include "proxy_manager.h"
#include <rpc/server.h>

#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#include "core/log/log_system.h"

ProxyRPC::~ProxyRPC()
{
    Clear();
}

void ProxyRPC::Initialize(const ProxyRPCInitInfo& init_info)
{
    Clear();

    Server = new rpc::server(init_info.Address, init_info.Port);
    LOG_INFO("ProxyRPC Server listen: {}:{}", init_info.Address, init_info.Port);

    BindProxyManager(init_info.Manager);

    Server->async_run();
    LOG_INFO("ProxyRPC Server is running.");
}

void ProxyRPC::Clear()
{
    if (Server)
    {
        Server->close_sessions();
        delete Server;
    }
}

#define PROXY_DEBUG_LOG(...) LOG_INFO(__VA_ARGS__)

ProxyActionResponse GNoResponse;

void ProxyRPC::BindProxyManager(ProxyManager* manager)
{
    Server->bind(PA_CREATE_OBJECT, [manager](ProxyObjectCreateDesc desc) {
        PROXY_DEBUG_LOG("ProxyAction: {}({})", PA_CREATE_OBJECT, desc.ClientHandle);
        ProxyActionDesc action_desc;
        action_desc.CreateDesc = desc;
        ProxyActionResponse response;
        manager->AddAction(PA_CREATE_OBJECT, action_desc, response);
        return response.ServerHandle;
    });

    Server->bind(PA_DESTROY_OBJECT, [manager](ProxyObjectDestroyDesc desc) {
        PROXY_DEBUG_LOG("ProxyAction: {}({})", PA_DESTROY_OBJECT, desc.ServerHandle);
        ProxyActionDesc action_desc;
        action_desc.DestroyDesc = desc;
        manager->AddAction(PA_DESTROY_OBJECT, action_desc, GNoResponse);
    });

    Server->bind(PA_UPDATE_OBJECT_TRANSFORM, [manager](ProxyTransformDesc desc) {
        PROXY_DEBUG_LOG("ProxyAction: {}({})", PA_UPDATE_OBJECT_TRANSFORM, desc.ServerHandle);
        ProxyActionDesc action_desc;
        action_desc.TransformDesc = desc;
        manager->AddAction(PA_UPDATE_OBJECT_TRANSFORM, action_desc, GNoResponse);
    });
}
