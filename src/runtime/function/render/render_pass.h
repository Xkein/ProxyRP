#pragma once

#include "rhi.h"
#include "function/render/render_resource.h"

#include <vector>

class WindowUI;
class RenderResourceManager;
class RenderPassCommon;
class RenderScene;

struct RenderPassInitInfo
{};

struct RenderPassCommonInfo
{
    std::shared_ptr<RHI>              RHI;
    std::shared_ptr<RenderPassCommon> PassCommon;
};

struct RenderPassPrepareInfo
{
    std::shared_ptr<RenderScene> Scene;
    std::shared_ptr<RenderResourceManager> ResourceManager;
};

struct FramebufferRecreateInfo
{
    RHIImageView* SceneAttachment;
    RHIImageView* UIAttachment;
    RHIImageView* PostProcessAttachment;
};


class RenderPass
{
public:
    virtual void Initialize(const RenderPassInitInfo* init_info) = 0;
    virtual void InitializeUIRenderBackend(WindowUI* window_ui) {}
    virtual void PostInitialize() {}
    virtual void SetCommonInfo(RenderPassCommonInfo* common_info);
    virtual void PrepareData(RenderPassPrepareInfo* prepare_info)                             = 0;
    virtual void UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) = 0;

    virtual void Draw() = 0;

public:
    struct _Framebuffer
    {
        int               Width;
        int               Height;
        RHIFramebufferRef Framebuffer;
        RHIRenderPassRef  RenderPass;
        
        std::vector<Texture> Attachments;
    };

protected:
    std::shared_ptr<RHI> RHI;

    std::shared_ptr<RenderPassCommon> PassCommon;

    _Framebuffer Framebuffer;

    std::vector<Descriptor> DescriptorInfos;
    std::vector<Pipeline>   RenderPipelines;

};
