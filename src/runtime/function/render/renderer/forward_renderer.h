#pragma once

#include "function/render/scene_renderer.h"

class ForwardSceneRenderer : public SceneRenderer
{
public:

    virtual void Initialize(RendererInitInfo* init_info) override;
    virtual void Clear() override;

    virtual void Render() override;

};
