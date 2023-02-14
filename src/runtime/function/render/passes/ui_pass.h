#pragma once

#include "function/render/render_pass.h"

struct UIPassInitInfo : RenderPassInitInfo
{
    RHIRenderPassRef Pass;
};

class UIPass : public RenderPass
{
public:
    void Initialize(const RenderPassInitInfo* init_info) override final;
    void InitializeUIRenderBackend(WindowUI* window_ui) override final;
    void Draw() override final;

private:
    void UploadFonts();

private:
    WindowUI* UI;
};
