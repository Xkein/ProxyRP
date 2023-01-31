#pragma once

#include "shader.h"
#include "shader_map.h"
#include "platform/platform.h"
#include "platform/string.h"

#include <taskflow/taskflow.hpp>

struct ShaderCompiledInfo
{
    std::vector<byte> ShaderData;

    String Message;
    bool   HasError;
};

class ShaderCompiler
{
public:
    static tf::Future<std::optional<ShaderCompiledInfo>> BeginCompileShader(ShaderType* shader_type, std::shared_ptr<ShaderMap> shader_map);

    static tf::Future<void> BeginCompileShaders(std::vector<ShaderType*> shader_types, std::shared_ptr<ShaderMap> shader_map);

private:
    static ShaderCompiledInfo Compile(ShaderType* shader_type, std::shared_ptr<ShaderMap> shader_map);
    static void               SaveShader(ShaderType* shader_type, ShaderCompiledInfo compiled_info);
};


tf::Future<void> __CompileAllShaders();

