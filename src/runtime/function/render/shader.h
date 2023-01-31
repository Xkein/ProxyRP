#pragma once

#include "platform/platform.h"
#include "platform/string.h"

#include <cstdint>
#include <vector>

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
    static std::vector<ShaderType*>& GetAllShaderTypeVector();
public:
    static std::vector<ShaderType*> GetAllShaderTypes();

    ShaderType(const String&    name,
               const String&    source_filename,
               const String&    enter_point,
               EShaderFrequency frequency);

    const String& GetHashedName() const;
    String        GetCachedFilePath() const;

	String           Name;
    String           TypeName;
    String           SourceFile;
    String           EntryPoint;
    EShaderFrequency Frequency;

    String HashedName;
};

class Shader
{
public:


    ShaderType* Type;
    int         Index {-1};
};

class ShaderCache
{
public:
};

template<typename... Ts>
struct ShaderPermutationDomain
{};


#define DECLARE_SHADER(ShaderClass) \
    static ShaderType StaticType;

#define IMPLEMENT_SHADER_TYPE(ShaderClass, SourceFilename, EntryPoint, Frequency) \
    ShaderType ShaderClass::StaticType(#ShaderClass, SourceFilename, EntryPoint, Frequency);


