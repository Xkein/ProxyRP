#pragma once

#include "render_swap_context.h"

#include <memory>

class RHI;
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
    std::shared_ptr<RHI>          GetRHI() const;
    std::shared_ptr<RenderCamera> GetRenderCamera() const;
    std::shared_ptr<RenderScene>  GetScene() const;

private:
    void ProcessSwapData();

    RenderSwapContext SwapContext;

    std::shared_ptr<RHI>      RHI;
    std::shared_ptr<RenderScene>    Scene;
    std::shared_ptr<RenderCamera>   Camera;
    std::shared_ptr<SceneRenderer>  Renderer;

    std::shared_ptr<RenderResourceManager> ResourceManager;
};
