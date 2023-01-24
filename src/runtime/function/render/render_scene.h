#pragma once

#include <memory>

class RenderCamera;

class RenderScene
{
public:
    void UpdateVisibleObjects(std::shared_ptr<RenderCamera> camera);


};
