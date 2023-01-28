#include "render_scene.h"
#include "render_common.h"
#include "render_util.h"
#include "render_resource_manager.h"
#include "core/macro.h"

void RenderScene::UpdateVisibleObjects(std::shared_ptr<RenderCamera> camera)
{
    UpdateVisibleObjectsLights(camera);
    UpdateVisibleObjectsCamera(camera);
}

void RenderScene::SetVisibleNodesReference() {}

void RenderScene::UpdateVisibleObjectsLights(std::shared_ptr<RenderCamera> camera)
{
    VisiableNodes.DirectionalLightVisibleMeshNodes = &DirectionalLightVisibleMeshNodes;
    VisiableNodes.PointLightsVisibleMeshNodes      = &PointLightsVisibleMeshNodes;
    VisiableNodes.MainCameraVisibleMeshNodes       = &MainCameraVisibleMeshNodes;
}

void RenderScene::UpdateVisibleObjectsCamera(std::shared_ptr<RenderCamera> camera)
{
    Matrix4x4 light_proj_view = CalculateDirectionalLightCamera(*this, *camera);

    LightProjView = light_proj_view;

    DirectionalLightVisibleMeshNodes.clear();

    ClusterFrustum frustum = CreateClusterFrustumFromMatrix(light_proj_view, -1.f, 1.f, -1.f, 1.f, 0.f, 1.f);

    for (const RenderEntity& entity : RenderEntities)
    {
        BoundingBox mesh_bounding_box = entity.BoundingBox;

        if (TiledFrustumIntersectBox(frustum, BoundingBoxTransform(mesh_bounding_box, entity.ModelMatrix)))
        {
            RenderMeshNode& node = DirectionalLightVisibleMeshNodes.emplace_back();

            node.ModelMatrix = &entity.ModelMatrix;

            ASSERT(entity.JointMatrices.size() <= MeshVertexBlendingMaxJointCount);

            if (!entity.JointMatrices.empty())
            {
                node.JointCount    = entity.JointMatrices.size();
                node.JointMatrices = entity.JointMatrices.data();
            }
            node.NodeId = entity.InstanceId;

            RenderMesh& mesh = 
        }
    }

}
