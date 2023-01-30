#include "shader.h"
#include "core/log/log_system.h"

std::vector<ShaderType*>& ShaderType::GetAllShaderTypeVector()
{
    static std::vector<ShaderType*> __AllShaderTypes;
    return __AllShaderTypes;
}

std::vector<ShaderType*> ShaderType::GetAllShaderTypes()
{
    return GetAllShaderTypeVector();
}

ShaderType::ShaderType(const String& name,
                       const String&    source_filename,
                       const String&    entry_point,
                       EShaderFrequency frequency) :
    Name(name), TypeName(name), SourceFile(source_filename), EntryPoint(entry_point), Frequency(frequency)
{
    GetAllShaderTypeVector().push_back(this);
}

const String& ShaderType::GetHashedName() const
{
    return HashedName;
}

