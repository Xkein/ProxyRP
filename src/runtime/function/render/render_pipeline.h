#pragma once

#include "render_pass.h"

class RenderPipeline
{
public:
    std::shared_ptr<RHI> RHI;

    std::shared_ptr<RenderPass> DirectionalLightPass;
    std::shared_ptr<RenderPass> PointLightShadowPass;


    std::shared_ptr<RenderPass> UIPass;
};
