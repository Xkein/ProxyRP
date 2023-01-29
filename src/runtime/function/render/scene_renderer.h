#pragma once

#include <memory>

class RHI;
class RenderScene;
class RenderCamera;
class RenderPipeline;

struct RendererInitInfo
{

};

class SceneRenderer
{
public:
    virtual void Initialize(RendererInitInfo* init_info) = 0;
    virtual void Clear() = 0;

    virtual void Render() = 0;

    virtual void UpdatePerBuffer(std::shared_ptr<RenderCamera> camera);

public:
    std::shared_ptr<RHI>      RHI;
    std::shared_ptr<RenderScene>    Scene;
    std::shared_ptr<RenderPipeline> Pipeline;
};
