#pragma once

#include "core/math/matrix.h"
#include "function/framework/object/object_id_allocator.h"
#include "platform/string.h"

#include <boost/functional/hash/hash.hpp>

enum class EGameObjectDescType
{
    Invalid,
    Mesh,
    Material,
    Transform,
    SkeletonBinding,
    SkeletonAnimation,
};

struct GameObjectMeshDesc
{
    String MeshFile;
};

struct SkeletonBindingDesc
{
    String SkeletonBindingFile;
};

struct SkeletonAnimationResultTransform
{
    Matrix4x4 Matrix;
};

struct SkeletonAnimationResult
{
    std::vector<SkeletonAnimationResultTransform> Transforms;
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

    GameObjectPartDesc(const GameObjectPartDesc& other) {
        *this = other;
    }

    ~GameObjectPartDesc()
    {
        if (Type == EGameObjectDescType::Mesh)
            MeshDesc.~GameObjectMeshDesc();
        else if (Type == EGameObjectDescType::Material)
            MaterialDesc.~GameObjectMaterialDesc();
        else if (Type == EGameObjectDescType::Transform)
            TransformDesc.~GameObjectTransformDesc();
    }

    GameObjectPartDesc& operator=(const GameObjectPartDesc& other)
    {
        if (Type == EGameObjectDescType::Mesh)
            MeshDesc = other.MeshDesc;
        else if (Type == EGameObjectDescType::Material)
            MaterialDesc = other.MaterialDesc;
        else if (Type == EGameObjectDescType::Transform)
            TransformDesc = other.TransformDesc;

        return *this;
    }

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
    GameObjectID GetId() const
    {
        return GoId;
    }
    const std::vector<GameObjectPartDesc>& GetObjectParts() const
    {
        return ObjectParts;
    }

    GameObjectID                    GoId {InvalidGObjectID};
    std::vector<GameObjectPartDesc> ObjectParts;
};

constexpr size_t InvalidPartId = std::numeric_limits<size_t>::max();

struct GameObjectPartId
{
    GameObjectID GoId {InvalidGObjectID};
    size_t       PartId {InvalidPartId};

    bool operator==(const GameObjectPartId& rhs) const
    {
        return GoId == rhs.GoId && PartId == rhs.PartId;
    }
    size_t GetHashValue() const
    {
        return GoId ^ (PartId << 1);
    }
    bool IsValid() const
    {
        return GoId != InvalidGObjectID && PartId != InvalidPartId;
    }
};

struct MeshSourceDesc
{
    String m_mesh_file;

    bool operator==(const MeshSourceDesc& rhs) const
    {
        return m_mesh_file == rhs.m_mesh_file;
    }
    size_t GetHashValue() const
    {
        return std::hash<String> {}(m_mesh_file);
    }
};

struct MaterialSourceDesc
{
    String m_base_color_file;
    String m_metallic_roughness_file;
    String m_normal_file;
    String m_occlusion_file;
    String m_emissive_file;

    bool operator==(const MaterialSourceDesc& rhs) const
    {
        return m_base_color_file == rhs.m_base_color_file && m_metallic_roughness_file == rhs.m_metallic_roughness_file && m_normal_file == rhs.m_normal_file &&
               m_occlusion_file == rhs.m_occlusion_file && m_emissive_file == rhs.m_emissive_file;
    }

    size_t GetHashValue() const
    {
        return boost::hash_value(std::tuple(m_base_color_file, m_metallic_roughness_file, m_normal_file, m_occlusion_file, m_emissive_file));
    }
};

template<>
struct std::hash<GameObjectPartId>
{
    size_t operator()(const GameObjectPartId& rhs) const noexcept
    {
        return rhs.GetHashValue();
    }
};
template<>
struct std::hash<MeshSourceDesc>
{
    size_t operator()(const MeshSourceDesc& rhs) const noexcept
    {
        return rhs.GetHashValue();
    }
};
template<>
struct std::hash<MaterialSourceDesc>
{
    size_t operator()(const MaterialSourceDesc& rhs) const noexcept
    {
        return rhs.GetHashValue();
    }
};



