#pragma once

#include "core/math/matrix.h"
#include "function/render/light.h"
#include "function/render/render_type.h"
#include "function/render/render_entity.h"
#include "function/render/render_object.h"
#include "function/render/guid_allocator.h"
#include "function/framework/object/object_id_allocator.h"

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
    friend class RenderSystem;

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

    void UpdateVisibleObjects(std::shared_ptr<RenderCamera> camera);

    void Clear();

    void SetVisibleNodesReference();

    GuidAllocator<GameObjectPartId>&   GetInstanceIdAllocator() { return InstanceIdAllocator; }
    GuidAllocator<MeshSourceDesc>&     GetMeshAssetIdAllocator() { return MeshAssetIdAllocator; }
    GuidAllocator<MaterialSourceDesc>& GetMaterialAssetdAllocator() { return MaterialAssetIdAllocator; }

    void         AddInstanceIdToMap(uint32_t instance_id, GameObjectID go_id);
    GameObjectID GetGObjectIDByMeshID(uint32_t mesh_id) const;
    void         DeleteEntityByGObjectID(GameObjectID go_id);

private:
    void UpdateVisibleObjectsLights(std::shared_ptr<RenderCamera> camera);
    void UpdateVisibleObjectsCamera(std::shared_ptr<RenderCamera> camera);

    GuidAllocator<GameObjectPartId>   InstanceIdAllocator;
    GuidAllocator<MeshSourceDesc>     MeshAssetIdAllocator;
    GuidAllocator<MaterialSourceDesc> MaterialAssetIdAllocator;

    std::unordered_map<uint32_t, GameObjectID> MeshObjectIdMap;

    std::unordered_map<MeshSourceDesc, BoundingBox> CachedBoundingBox;

    std::shared_ptr<RenderResourceManager> ResourceManager;
};
