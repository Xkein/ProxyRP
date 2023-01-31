#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

CLASS(ProxyManager)
{
    REFLECTION_CLASS_BODY(ProxyManager);

public:

    PROPERTY()
    String PipeName;

    std::vector<std::shared_ptr<GameObject>> ProxyGObjects;
};

