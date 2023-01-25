#pragma once

#include "core/math/matrix.h"
#include "function/framework/object/object_id_allocator.h"
#include "platform/string.h"

enum class EGameObjectDescType
{
    Invalid,
    Mesh,
    Material,
    Transform,
};

struct GameObjectMeshDesc
{
    String MeshFile;
};

struct GameObjectMaterialDesc
{
    String BaseColorTexture;
    String NormalTexture;
    String MetallicRoughnessTexture;
    String OcclusionTexture;
    String EmissiveTexture;
};

struct GameObjectTransformDesc
{
    Matrix4x4 TransformMatrix {Matrix4x4::Identity()};
};

struct GameObjectPartDesc
{
    GameObjectPartDesc(const GameObjectMeshDesc& mesh_desc) : Type(EGameObjectDescType::Mesh), MeshDesc(mesh_desc) {}

    GameObjectPartDesc(const GameObjectMaterialDesc& material_desc) :
        Type(EGameObjectDescType::Material), MaterialDesc(material_desc)
    {}

    GameObjectPartDesc(const GameObjectTransformDesc& transform_desc) :
        Type(EGameObjectDescType::Transform), TransformDesc(transform_desc)
    {}

    EGameObjectDescType Type {EGameObjectDescType::Invalid};
    union
    {
        GameObjectMeshDesc      MeshDesc;
        GameObjectMaterialDesc  MaterialDesc;
        GameObjectTransformDesc TransformDesc;
    };
};

struct GameObjectDesc
{
    GameObjectID                    GoId {InvalidGObjectID};
    std::vector<GameObjectPartDesc> ObjectParts;
};

