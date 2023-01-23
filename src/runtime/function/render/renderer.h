#pragma once

#include <memory>

class VulkanRHI;
class RenderScene;
class RenderCamera;
class RenderPipeline;

struct RendererInitInfo
{

};

class Renderer
{
public:
    virtual void Initialize(RendererInitInfo* init_info) = 0;
    virtual void Clear() = 0;

    virtual void Render() = 0;


public:
    std::shared_ptr<VulkanRHI>      RHI;
    std::shared_ptr<RenderCamera>   Camera;
    std::shared_ptr<RenderScene>    Scene;
    std::shared_ptr<RenderPipeline> Pipeline;
};
