#pragma once

#include "function/render/render_pass.h"
#include "function/render/render_type.h"
#include "function/render/render_common.h"

struct MeshPassInitInfo : public RenderPassInitInfo
{
    RHIImageViewRef DirectionalLightShadowMap;
    RHIRenderPassRef RenderPass;
};

class MeshRenderPass : public RenderPass
{
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
    void SetupFramebufferDescriptorSet();
    void SetupDescriptorSetLayout();
    void SetupDescriptorSet();
    void SetupPipelines();

    void DrawMeshLighting();

    std::vector<RHIFramebufferRef> SwapchainFramebuffers;

    RHIImageViewRef DirectionalLightShadowMap;
    VisiableNodes*  VisiableNodes;

    MeshPerframeStorageBufferObject PerframeStorageBufferObject;
};

