#pragma once

#include "render_swap_context.h"

#include <memory>

class VulkanRHI;
class RenderScene;
class RenderCamera;
class Renderer;
class RenderPipeline;
class WindowSystem;

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

private:
    void ProcessSwapData();

    RenderSwapContext SwapContext;

    std::shared_ptr<VulkanRHI>      RHI;
    std::shared_ptr<RenderScene>    Scene;
    std::shared_ptr<RenderCamera>   Camera;
    std::shared_ptr<Renderer>       Renderer;
    std::shared_ptr<RenderPipeline> Pipeline;
};
