#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

#include <vector>

class Component;

STRUCT()
struct ObjectDefinitionResource
{
    REFLECTION_STRUCT_BODY(ObjectDefinitionResource)

    PROPERTY()
    std::vector<Instance<Component>> Components;
};

STRUCT()
struct ObjectInstanceResource
{
    REFLECTION_STRUCT_BODY(ObjectInstanceResource)

    String Name;
    String DefinitionUrl;

    PROPERTY()
    std::vector<Instance<Component>> InstancedComponents;
};
