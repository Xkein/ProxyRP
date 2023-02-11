#include "proxy_rpc.h"

ProxyRPC::~ProxyRPC()
{
    Clear();
}

void ProxyRPC::Initialize(const ProxyRPCInitInfo& init_info)
{
    Server = new rpc::server(init_info.Port);


}

void ProxyRPC::Clear()
{
    if (Server)
    {
        Server->close_sessions();
        delete Server;
    }
}
