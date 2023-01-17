#pragma once

#include "rhi_resource.h"

class WindowSystem;

struct RHIInitInfo
{
    std::shared_ptr<WindowSystem> WindowSystem;
};

class RHI
{
public:
    virtual ~RHI() = 0;

    virtual void Initialize(RHIInitInfo init_info) = 0;
    
    // allocate and create

    virtual void CreateSwapChain() = 0;
    virtual void RecreateSwapChain() = 0;
    virtual void CreateSwapChainImageViews() = 0;

    // command

    // query

    // destroy
    virtual void Clear() = 0;
    virtual void ClearSwapChain() = 0;

    // memory

    // synchronize
};
