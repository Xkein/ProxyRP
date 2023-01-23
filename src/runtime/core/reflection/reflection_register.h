#pragma once

#include "platform/string.h"

#include <rttr/registration>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct __SerializerMethods
{
    void* (*ConstructWithJson)(const json& context);
    bool (*WriteJson)(void* instance, json& context);
};

class TypeMetaRegister
{
public:
    static void Register();
    static void Unregister();

    static void __RegisterSerializerMethods(const StringView& name, __SerializerMethods methods);
};
