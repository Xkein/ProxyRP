#pragma once

#include <memory>

class WindowSystem;
class RenderSystem;

struct WindowUIInitInfo
{
    std::shared_ptr<WindowSystem> WindowSystem;
    std::shared_ptr<RenderSystem> RenderSystem;
};

class WindowUI
{
public:
    virtual void Initialize(WindowUIInitInfo init_info) = 0;
    virtual void PreRender()                            = 0;
};


