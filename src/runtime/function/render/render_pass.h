#pragma once

#include "rhi.h"

class WindowUI;

struct RenderPassInitInfo
{};

struct RenderPassCommonInfo
{
    std::shared_ptr<RHI> RHI;
};

class RenderPass
{
public:
    virtual void Initialize(const RenderPassInitInfo* init_info) = 0;
    virtual void PostInitialize() {}
    virtual void SetCommonInfo(RenderPassCommonInfo common_info);
    //virtual void PreparePassData(std::shared_ptr<RenderResourceBase> render_resource) {}
    virtual void InitializeUIRenderBackend(WindowUI* window_ui) {}


protected:
    std::shared_ptr<RHI> RHI;
};
