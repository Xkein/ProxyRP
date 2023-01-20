#pragma once

#include <memory>

class RHI;
class RenderScene;

class Renderer
{
public:

    virtual void Render() = 0;


public:
    std::shared_ptr<RHI>         RHI;
    std::shared_ptr<RenderScene> Scene;
};
