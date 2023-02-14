#pragma once

#include <memory>

class RHI;
class RenderScene;
class RenderCamera;
class RenderPipeline;
class WindowUI;
class RenderResourceManager;

struct RendererInitInfo
{
    std::shared_ptr<RHI>                   RHI;
    std::shared_ptr<RenderScene>           Scene;
    std::shared_ptr<RenderResourceManager> ResourceManager;
};

class SceneRenderer
{
public:
    virtual void Initialize(RendererInitInfo* init_info) = 0;
    virtual void Clear() = 0;

    virtual void Render() = 0;

    virtual void PrepareData();

    virtual void UpdatePerBuffer(std::shared_ptr<RenderCamera> camera);

    virtual void InitializeUIRenderBackend(WindowUI* window_ui);

public:
    std::shared_ptr<RHI>      RHI;
    std::shared_ptr<RenderScene>    Scene;
    std::shared_ptr<RenderPipeline> Pipeline;

    std::shared_ptr<RenderResourceManager> ResourceManager;
};
