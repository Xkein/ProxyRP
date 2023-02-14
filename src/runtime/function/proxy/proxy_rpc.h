#pragma once

#include <cstdint>

class ProxyManager;

namespace rpc
{
    class server;
}

struct ProxyRPCInitInfo
{
    const char*   Address;
    uint16_t      Port;
    ProxyManager* Manager;
};

class ProxyRPC
{
public:
    ~ProxyRPC();

    void Initialize(const ProxyRPCInitInfo& init_info);
    void Clear();

private:
    void BindProxyManager(ProxyManager* manager);

    rpc::server* Server;
};
