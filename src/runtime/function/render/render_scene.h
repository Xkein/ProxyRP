#pragma once

#include "core/math/matrix.h"
#include "function/render/light.h"
#include "function/render/render_type.h"
#include "function/render/render_entity.h"

#include <memory>

class RenderCamera;
class RenderResourceManager;
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

    Matrix4x4 LightProjView;
    
    std::vector<RenderEntity> RenderEntities;
    
    std::vector<RenderMeshNode> DirectionalLightVisibleMeshNodes;
    std::vector<RenderMeshNode> PointLightsVisibleMeshNodes;
    std::vector<RenderMeshNode> MainCameraVisibleMeshNodes;

    VisiableNodes VisiableNodes;

    void SetVisibleNodesReference();

    void UpdateVisibleObjects(std::shared_ptr<RenderCamera> camera);

private:
    void UpdateVisibleObjectsLights(std::shared_ptr<RenderCamera> camera);
    void UpdateVisibleObjectsCamera(std::shared_ptr<RenderCamera> camera);


    std::shared_ptr<RenderResourceManager> ResourceManager;
};
