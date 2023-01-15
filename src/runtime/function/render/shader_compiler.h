#pragma once

#include "shader.h"
#include "platform/string.h"

class RHIShader;

RHIShader* CompileShader(const Char* path);

struct ShaderCompileInfo
{
    String CodePath;
    String OutputPath;

    String    EntryPoint;
    ShaderLanguage Language;
    ShaderType     Type;
};

struct ShaderCompileResult
{
    bool Success;
};


ShaderCompileResult CompileShader(const ShaderCompileInfo& info);

RHIShader* LoadShader(const String& shader_path);

