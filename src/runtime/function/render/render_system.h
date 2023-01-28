#pragma once

#include "render_swap_context.h"

#include <memory>

class VulkanRHI;
class RenderScene;
class RenderCamera;
class SceneRenderer;
class RenderPipeline;
class WindowSystem;
class RenderResourceManager;
class WindowUI;

struct RenderSystemInitInfo
{
    std::shared_ptr<WindowSystem> WindowSystem;
};

class RenderSystem
{
public:
    RenderSystem() = default;
    ~RenderSystem();

    void Initialize(RenderSystemInitInfo init_info);
    void Clear();

    void Tick(float delta_time);
    
    void InitializeUIRenderBackend(WindowUI* window_ui);

    void                          SwapLogicRenderData();
    RenderSwapContext&            GetSwapContext();
    std::shared_ptr<VulkanRHI>    GetRHI() const;
    std::shared_ptr<RenderCamera> GetRenderCamera() const;

private:
    void ProcessSwapData();

    RenderSwapContext SwapContext;

    std::shared_ptr<VulkanRHI>      RHI;
    std::shared_ptr<RenderScene>    Scene;
    std::shared_ptr<RenderCamera>   Camera;
    std::shared_ptr<SceneRenderer>  Renderer;
    std::shared_ptr<RenderPipeline> Pipeline;

    std::shared_ptr<RenderResourceManager> ResourceManager;
};
