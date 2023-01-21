#pragma once

#include "rhi.h"

class VulkanRHI;
class WindowUI;

struct RenderPassInitInfo
{};

struct RenderPassCommonInfo
{
    std::shared_ptr<VulkanRHI> RHI;
};

class RenderPass
{
public:
    virtual void Initialize(const RenderPassInitInfo* init_info) = 0;
    virtual void PostInitialize() {}
    virtual void SetCommonInfo(RenderPassCommonInfo common_info);
    virtual void PrepareData() = 0;
    virtual void UpdateAfterFramebufferRecreate() = 0;

protected:
    std::shared_ptr<VulkanRHI> RHI;
};
