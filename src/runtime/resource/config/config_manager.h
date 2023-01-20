#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

STRUCT(EngineConfig)
{
    REFLECTION_STRUCT_BODY(EngineConfig);

};

STRUCT(GraphicsConfig)
{
    REFLECTION_STRUCT_BODY(GraphicsConfig);

    PROPERTY()
    bool EnableFxaa {false};
};

STRUCT(GlobalConfig)
{
    REFLECTION_STRUCT_BODY(GlobalConfig);

    PROPERTY()
    String GlobalRenderingResourceUrl;
};

class ConfigManager
{
public:
    void Initialize();

    GlobalConfig   Global;
    EngineConfig   Engine;
    GraphicsConfig Graphics;
};
