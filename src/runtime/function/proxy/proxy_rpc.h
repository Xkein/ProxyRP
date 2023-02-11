#pragma once

#include <rpc/server.h>

struct ProxyRPCInitInfo
{
    uint16_t Port;
};

class ProxyRPC
{
public:
    ~ProxyRPC();

    void Initialize(const ProxyRPCInitInfo& init_info);

    void Clear();

    rpc::server* Server;
};
