#pragma once

#include "platform/string.h"

#include <cstdint>

enum EShaderFrequency : uint8_t
{
    SF_Vertex        = 0,
    SF_Pixel         = 3,
    SF_Geometry      = 4,
    SF_Compute       = 5,
    SF_RayGen        = 6,
    SF_RayMiss       = 7,
    SF_RayHitGroup   = 8,
    SF_RayCallable   = 9,
};

class ShaderType
{
public:

	String           Name;
    String           TypeName;
    String           SourceFile;
    String           EnterPoint;
    EShaderFrequency Frequency;
};

class Shader
{
public:

    void Finalize();

private:
    std::shared_ptr<ShaderType> Type;
    int                         Index;
};

class ShaderCache
{
public:

};


