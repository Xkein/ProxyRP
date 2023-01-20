#pragma once

#include "render_swap_context.h"

#include <memory>

class RHI;
class RenderScene;
class Renderer;
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

    std::shared_ptr<RHI>         RHI;
    std::shared_ptr<RenderScene> RenderScene;
    std::shared_ptr<Renderer>    Renderer;
};
