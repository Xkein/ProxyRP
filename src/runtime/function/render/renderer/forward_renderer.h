#pragma once

#include "function/render/renderer.h"

class ForwardRenderer : public Renderer
{
public:

    virtual void Initialize(RendererInitInfo* init_info) override;
    virtual void Clear() override;

    virtual void Render() override;

};
