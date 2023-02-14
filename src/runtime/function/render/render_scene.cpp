#include "render_scene.h"
#include "render_common.h"
#include "render_util.h"
#include "render_resource_manager.h"
#include "function/render/render_camera.h"
#include "core/macro.h"
#include "function/render/scene_proxy.h"
#include "function/framework/component/light/point_light_component.h"

void RenderScene::AddLight(LightComponent* light)
{
    LightSceneProxy* proxy = light->CreateSceneProxy();
    if (proxy)
    {
        if (light->GetLightType() == ELightType::Point)
        {
            PointLightList.push_back((PointLightSceneProxy*)proxy);
        }

        light->SceneProxy = proxy;
    }
}

void RenderScene::RemoveLight(LightComponent* light)
{
    if (light->SceneProxy)
    {
        if (light->GetLightType() == ELightType::Point)
        {
            PointLightList.erase(std::find(PointLightList.begin(), PointLightList.end(), light->SceneProxy));
        }

        delete light->SceneProxy;
        light->SceneProxy = nullptr;
    }
}

void RenderScene::UpdateLight(LightComponent* light)
{
    LightSceneProxy* proxy = light->SceneProxy;
    if (proxy == nullptr)
        return;
    
    proxy->Position = light->GetPosition();
    proxy->Color = light->GetLightColor();
    proxy->Intensity = light->GetIntensity();

    if (light->GetLightType() == ELightType::Point)
    {
        PointLightComponent*  point_light       = static_cast<PointLightComponent*>(light);
        PointLightSceneProxy* point_light_proxy = static_cast<PointLightSceneProxy*>(proxy);

        point_light_proxy->Radius = point_light->GetRadius();
    }
}

void RenderScene::UpdateVisibleObjects(std::shared_ptr<RenderCamera> camera)
{
    UpdateVisibleObjectsLights(camera);
    UpdateVisibleObjectsCamera(camera);
}

void RenderScene::SetVisibleNodesReference()
{
    VisiableNodes.DirectionalLightVisibleMeshNodes = &DirectionalLightVisibleMeshNodes;
    VisiableNodes.PointLightsVisibleMeshNodes      = &PointLightsVisibleMeshNodes;
    VisiableNodes.MainCameraVisibleMeshNodes       = &MainCameraVisibleMeshNodes;
}

void RenderScene::UpdateVisibleObjectsLights(std::shared_ptr<RenderCamera> camera)
{
    // update directional light
    {
        Matrix4x4 light_proj_view = CalculateDirectionalLightCamera(*this, *camera);

        DirectionalLightShadowPerframeStorageBufferObject.LightProjView    = light_proj_view;
        PerframeStorageBufferObject.DirectionalLightProjView = light_proj_view;

        DirectionalLightVisibleMeshNodes.clear();

        ClusterFrustum frustum = CreateClusterFrustumFromMatrix(light_proj_view, -1.f, 1.f, -1.f, 1.f, 0.f, 1.f);

        for (const RenderEntity& entity : RenderEntities)
        {
            BoundingBox mesh_bounding_box = entity.BoundingBox;

            if (TiledFrustumIntersectBox(frustum, BoundingBoxTransform(mesh_bounding_box, entity.ModelMatrix)))
            {
                RenderMeshNode& node = DirectionalLightVisibleMeshNodes.emplace_back();

                node.ModelMatrix = &entity.ModelMatrix;

                ASSERT(entity.JointMatrices.size() <= GMeshVertexBlendingMaxJointCount);

                if (!entity.JointMatrices.empty())
                {
                    node.JointCount    = entity.JointMatrices.size();
                    node.JointMatrices = entity.JointMatrices.data();
                }
                node.NodeId = entity.InstanceId;

                std::shared_ptr<RenderMesh> mesh = ResourceManager->GetEntityMesh(entity);
                node.RefMesh                     = mesh.get();
                node.EnableVertexBlending        = entity.EnableVertexBlending;

                std::shared_ptr<PBRMaterial> material = ResourceManager->GetEntityMaterial(entity);
                node.RefMaterial                      = material.get();
            }
        }
    }

    // update point lights
    {
        PointLightsVisibleMeshNodes.clear();

        uint32_t                    point_light_num = PointLightList.size();
        std::vector<BoundingSphere> point_lights_bounding_spheres(point_light_num);
        for (size_t i = 0; i < point_light_num; i++)
        {
            point_lights_bounding_spheres[i].Center = PointLightList[i]->Position;
            point_lights_bounding_spheres[i].Radius = PointLightList[i]->Radius;
        }

        for (const RenderEntity& entity : RenderEntities)
        {
            BoundingBox mesh_bounding_box = entity.BoundingBox;

            bool intersect_with_point_light = false;
            for (size_t i = 0; i < point_light_num; i++)
            {
                BoundingBox transformed_bounding_box = BoundingBoxTransform(mesh_bounding_box, entity.ModelMatrix);
                if (BoxIntersectsWithSphere(transformed_bounding_box, point_lights_bounding_spheres[i]))
                {
                    intersect_with_point_light = true;
                    break;
                }
            }

            if (intersect_with_point_light)
            {
                RenderMeshNode& node = PointLightsVisibleMeshNodes.emplace_back();

                node.ModelMatrix = &entity.ModelMatrix;

                ASSERT(entity.JointMatrices.size() <= GMeshVertexBlendingMaxJointCount);

                if (!entity.JointMatrices.empty())
                {
                    node.JointCount    = entity.JointMatrices.size();
                    node.JointMatrices = entity.JointMatrices.data();
                }
                node.NodeId = entity.InstanceId;

                std::shared_ptr<RenderMesh> mesh = ResourceManager->GetEntityMesh(entity);
                node.RefMesh                     = mesh.get();
                node.EnableVertexBlending        = entity.EnableVertexBlending;

                std::shared_ptr<PBRMaterial> material = ResourceManager->GetEntityMaterial(entity);
                node.RefMaterial                      = material.get();
            }
        }
    }
}

void RenderScene::UpdateVisibleObjectsCamera(std::shared_ptr<RenderCamera> camera)
{
    MainCameraVisibleMeshNodes.clear();

    Matrix4x4 view_matrix      = camera->GetViewMatrix();
    Matrix4x4 proj_matrix      = camera->GetPerspectiveMatrix();
    Matrix4x4 proj_view_matrix = proj_matrix * view_matrix;

    ClusterFrustum frustum = CreateClusterFrustumFromMatrix(proj_view_matrix, -1.f, 1.f, -1.f, 1.f, 0.f, 1.f);

    for (const auto& entity : RenderEntities)
    {
        BoundingBox mesh_bounding_box = entity.BoundingBox;

        if (TiledFrustumIntersectBox(frustum, BoundingBoxTransform(mesh_bounding_box, entity.ModelMatrix)))
        {
            RenderMeshNode& node = MainCameraVisibleMeshNodes.emplace_back();

            node.ModelMatrix = &entity.ModelMatrix;

            ASSERT(entity.JointMatrices.size() <= GMeshVertexBlendingMaxJointCount);

            if (!entity.JointMatrices.empty())
            {
                node.JointCount    = entity.JointMatrices.size();
                node.JointMatrices = entity.JointMatrices.data();
            }
            node.NodeId = entity.InstanceId;

            std::shared_ptr<RenderMesh> mesh = ResourceManager->GetEntityMesh(entity);
            node.RefMesh                     = mesh.get();
            node.EnableVertexBlending        = entity.EnableVertexBlending;

            std::shared_ptr<PBRMaterial> material = ResourceManager->GetEntityMaterial(entity);
            node.RefMaterial                      = material.get();
        }
    }
}


void RenderScene::Clear()
{
    InstanceIdAllocator.Clear();
    MeshObjectIdMap.clear();
    RenderEntities.clear();
}

void RenderScene::AddInstanceIdToMap(uint32_t instance_id, GameObjectID go_id)
{
    MeshObjectIdMap[instance_id] = go_id;
}

GameObjectID RenderScene::GetGObjectIDByMeshID(uint32_t mesh_id) const {
    if (auto iter = MeshObjectIdMap.find(mesh_id); iter != MeshObjectIdMap.end())
    {
        return iter->second;
    }

    return InvalidGObjectID;
}

void RenderScene::DeleteEntityByGObjectID(GameObjectID go_id)
{
    for (auto it = MeshObjectIdMap.begin(); it != MeshObjectIdMap.end(); it++)
    {
        if (it->second == go_id)
        {
            MeshObjectIdMap.erase(it);
            break;
        }
    }

    GameObjectPartId part_id = {go_id, 0};
    size_t           find_guid;
    if (InstanceIdAllocator.GetElementGuid(part_id, find_guid))
    {
        for (auto it = RenderEntities.begin(); it != RenderEntities.end(); it++)
        {
            if (it->InstanceId == find_guid)
            {
                RenderEntities.erase(it);
                break;
            }
        }
    }
}
