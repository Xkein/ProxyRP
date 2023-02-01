#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

CLASS(ProxyManager)
{
    REFLECTION_CLASS_BODY(ProxyManager);

public:
    void CreateProxyGameObject();


    PROPERTY()
    String PipeName;

    std::vector<std::shared_ptr<GameObject>> ProxyGObjects;
};

