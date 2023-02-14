#pragma once

#include "core/math/matrix.h"
#include "core/math/axis_aligned.h"
#include "function/framework/object/object_id_allocator.h"
#include "platform/string.h"

#include <boost/functional/hash/hash.hpp>

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
    bool   WithTexture {false};
};

struct GameObjectTransformDesc
{
    Matrix4x4 TransformMatrix {Matrix4x4::Identity()};
};

struct GameObjectPartDesc
{
    GameObjectMeshDesc      MeshDesc;
    GameObjectMaterialDesc  MaterialDesc;
    GameObjectTransformDesc TransformDesc;
    bool                    WithAnimation {false};
    SkeletonBindingDesc     SkeletonBindingDesc;
    SkeletonAnimationResult SkeletonAnimationResult;
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
    String MeshFile;

    bool operator==(const MeshSourceDesc& rhs) const
    {
        return MeshFile == rhs.MeshFile;
    }
    size_t GetHashValue() const
    {
        return std::hash<String> {}(MeshFile);
    }
};

struct MaterialSourceDesc
{
    String BaseColorFile;
    String NormalFile;
    String MetallicRoughnessFile;
    String OcclusionFile;
    String EmissiveFile;

    bool operator==(const MaterialSourceDesc& rhs) const
    {
        return BaseColorFile == rhs.BaseColorFile && MetallicRoughnessFile == rhs.MetallicRoughnessFile && NormalFile == rhs.NormalFile && OcclusionFile == rhs.OcclusionFile &&
               EmissiveFile == rhs.EmissiveFile;
    }

    size_t GetHashValue() const
    {
        return boost::hash_value(std::tuple(BaseColorFile, MetallicRoughnessFile, NormalFile, OcclusionFile, EmissiveFile));
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



