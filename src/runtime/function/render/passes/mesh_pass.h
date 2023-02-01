#pragma once

#include "function/render/render_pass.h"
#include "function/render/render_type.h"
#include "function/render/render_common.h"

struct MeshPassInitInfo : public RenderPassInitInfo
{
    RHIImageViewRef DirectionalLightShadowMap;
};

class MeshPass : public RenderPass
{
    enum
    {
        _pass_attachment_depth,
        _pass_attachment_swap_chain_image,
        _pass_attachment_count,
    };

    enum
    {
        _subpass_forward_lighting,
        _subpass_count,
    };

    enum
    {
        _layout_per_mesh,
        _layout_mesh_global,
        _layout_mesh_per_material,
        _layout_count,
    };

    enum
    {
        _pipeline_mesh_lighting,
        _pipeline_count,
    };

public:
    virtual void Initialize(const RenderPassInitInfo* init_info) override;
    virtual void PostInitialize() override;
    virtual void PrepareData(RenderPassPrepareInfo* prepare_info) override;
    virtual void UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) override;
    virtual void Draw() override;

private:
    void SetupAttachments();
    void SetupFramebuffer();
    void SetupFramebufferDescriptorSet();
    void SetupDescriptorSetLayout();
    void SetupDescriptorSet();
    void SetupRenderPass();
    void SetupPipelines();

    void DrawMeshLighting();

    std::vector<RHIFramebufferRef> SwapchainFramebuffers;

    RHIImageViewRef DirectionalLightShadowMap;
    VisiableNodes*  VisiableNodes;

    MeshPerframeStorageBufferObject PerframeStorageBufferObject;
};

