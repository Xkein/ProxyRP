
#include "function/render/render_common.h"
#include "function/render/render_pass.h"
#include "function/render/render_type.h"

struct MainPassInitInfo : public RenderPassInitInfo
{
    std::shared_ptr<RenderPass> MeshPass;
    std::shared_ptr<RenderPass> FxaaPass;
    std::shared_ptr<RenderPass> ColorGradingPass;
    std::shared_ptr<RenderPass> ToneMappingPass;
    std::shared_ptr<RenderPass> UIPass;
};

class MainRenderPass : public RenderPass
{
public:
    virtual void Initialize(const RenderPassInitInfo* init_info) override;
    virtual void PostInitialize() override;
    virtual void PrepareData(RenderPassPrepareInfo* prepare_info) override;
    virtual void UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) override;
    virtual void Draw() override;

    RHIRenderPassRef GetRenderPass() const;
    std::vector<RHIImageViewRef> GetFramebufferImageViews() const;


private:
    void SetupAttachments();
    void SetupFramebuffer();
    void SetupFramebufferDescriptorSet();
    void SetupRenderPass();

    std::vector<RHIFramebufferRef> SwapchainFramebuffers;

    std::shared_ptr<RenderPass> MeshPass;
    std::shared_ptr<RenderPass> FxaaPass;
    std::shared_ptr<RenderPass> ColorGradingPass;
    std::shared_ptr<RenderPass> ToneMappingPass;
    std::shared_ptr<RenderPass> UIPass;
};
