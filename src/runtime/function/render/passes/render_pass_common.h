#pragma once

#include "function/render/render_pass.h"

class RenderPassCommon
{
public:
    void SetupDescriptorSetLayout();

    std::shared_ptr<VulkanRHI> RHI;

    RHIDescriptorSetLayoutRef PerMeshLayout;
};
