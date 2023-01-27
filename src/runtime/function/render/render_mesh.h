#pragma once

#include "core/math/vector.h"
#include "function/render/rhi_struct.h"

struct MeshVertex
{
    struct MeshVertexPostition
    {
        Vector3f Position;
    };

    struct MeshVertexVaryingEnableBlending
    {
        Vector3f Normal;
        Vector3f Tangent;
    };

    struct MeshVertexVarying
    {
        Vector2f Texcoord;
    };

    struct MeshVertexJointBinding
    {
        int      Indices[4];
        Vector4f Weights;
    };

    static std::array<RHIVertexInputBindingDescription, 3> GetBindingDescriptions() {
        std::array<RHIVertexInputBindingDescription, 3> binding_descriptions {};
        // position
        binding_descriptions[0].binding   = 0;
        binding_descriptions[0].stride    = sizeof(MeshVertexPostition);
        binding_descriptions[0].inputRate = RHIVertexInputRate::eVertex;
        // varying blending
        binding_descriptions[1].binding   = 1;
        binding_descriptions[1].stride    = sizeof(MeshVertexVaryingEnableBlending);
        binding_descriptions[1].inputRate = RHIVertexInputRate::eVertex;
        // varying
        binding_descriptions[2].binding   = 2;
        binding_descriptions[2].stride    = sizeof(MeshVertexVarying);
        binding_descriptions[2].inputRate = RHIVertexInputRate::eVertex;

        return binding_descriptions;
    }

    static std::array<RHIVertexInputAttributeDescription, 4> GetAttributeDescription()
    {
        std::array<RHIVertexInputAttributeDescription, 4> attribute_descriptions {};
        // position
        attribute_descriptions[0].binding  = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format   = RHIFormat::eR32G32B32Sfloat;
        attribute_descriptions[0].offset   = offsetof(MeshVertexPostition, Position);
        // varying blending
        attribute_descriptions[1].binding  = 1;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format   = RHIFormat::eR32G32B32Sfloat;
        attribute_descriptions[1].offset   = offsetof(MeshVertexVaryingEnableBlending, Normal);
        attribute_descriptions[2].binding  = 1;
        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].format   = RHIFormat::eR32G32B32Sfloat;
        attribute_descriptions[2].offset   = offsetof(MeshVertexVaryingEnableBlending, Tangent);

        attribute_descriptions[3].binding  = 2;
        attribute_descriptions[3].location = 3;
        attribute_descriptions[3].format   = RHIFormat::eR32G32B32Sfloat;
        attribute_descriptions[3].offset   = offsetof(MeshVertexVarying, Texcoord);

        return attribute_descriptions;
    }
};
