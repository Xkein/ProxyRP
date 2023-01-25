#pragma once

#include "function/render/light.h"

#include <memory>

class RenderCamera;

class BufferData;

struct PointLightList
{
    std::vector<PointLight> Lights;

    std::shared_ptr<BufferData> Buffer;
};

class RenderScene
{
public:
    struct
    {
        AmbientLight     Ambient;
        DirectionalLight Directional;
        PointLightList   PointList;
    } Light;

    void UpdateVisibleObjects(std::shared_ptr<RenderCamera> camera);

private:
    void UpdateVisibleObjectsLights(std::shared_ptr<RenderCamera> camera);
    void UpdateVisibleObjectsCamera(std::shared_ptr<RenderCamera> camera);

};
