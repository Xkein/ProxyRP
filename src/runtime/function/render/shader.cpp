#include "shader.h"

ShaderType::ShaderType(const String&    name,
                       const String&    source_filename,
                       const String&    entry_point,
                       EShaderFrequency frequency) :
    Name(name), TypeName(name), SourceFile(source_filename), EntryPoint(entry_point), Frequency(frequency)
{
    __AllShaderTypes.push_back(this);
}

const String& ShaderType::GetHashedName() const
{
    return HashedName;
}

