#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"

ENUM(ERenderPipelineType : uint8_t, AsInteger) {
    Forward,
    Deferred,
};

STRUCT(GlobalRenderingResource)
{
    REFLECTION_STRUCT_BODY(GlobalRenderingResource);

    PROPERTY()
    ERenderPipelineType RenderPipelineType;
};
