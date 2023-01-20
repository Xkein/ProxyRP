#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

#include <vector>

class Component;

STRUCT(ObjectDefinitionResource)
{
    REFLECTION_STRUCT_BODY(ObjectDefinitionResource);

    PROPERTY()
    std::vector<Instance<Component>> Components;
};

STRUCT(ObjectInstanceResource)
{
    REFLECTION_STRUCT_BODY(ObjectInstanceResource);

    PROPERTY()
    String Name;
    PROPERTY()
    String DefinitionUrl;
    PROPERTY()
    std::vector<Instance<Component>> InstancedComponents;
};
