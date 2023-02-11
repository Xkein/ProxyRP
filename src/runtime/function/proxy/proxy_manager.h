#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"
#include "proxy_rpc.h"

CLASS(ProxyManager)
{
    REFLECTION_CLASS_BODY(ProxyManager);

public:
    void CreateProxyGameObject();


    PROPERTY()
    String Port;

    ProxyRPC RPC;

    std::vector<std::shared_ptr<GameObject>> ProxyGObjects;
};

