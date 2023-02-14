#pragma once

#include <rpc/msgpack.hpp>
#include <stdint.h>

struct ProxyObjectCreateDesc
{
    uint64_t    ClientHandle;
    std::string ObjectName;
    std::string ObjectDefinitionUrl;
    MSGPACK_DEFINE_ARRAY(ClientHandle, ObjectName, ObjectDefinitionUrl);
};

struct ProxyObjectDestroyDesc
{
    uint64_t ServerHandle;
    MSGPACK_DEFINE_ARRAY(ServerHandle);
};

struct ProxyTransformDesc
{
    uint64_t ServerHandle;
    float    Position[3];
    float    Scale[3];
    float    Quaternion[4];

    MSGPACK_DEFINE_ARRAY(ServerHandle, Position, Scale, Quaternion);
};
