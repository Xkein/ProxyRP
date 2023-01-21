#pragma once

#include <memory>

class VulkanRHI;
class RenderScene;
class RenderCamera;

class Renderer
{
public:

    virtual void Render() = 0;

    virtual void Clear() = 0;

public:
    std::shared_ptr<VulkanRHI>    RHI;
    std::shared_ptr<RenderCamera> Camera;
    std::shared_ptr<RenderScene>  Scene;
};
