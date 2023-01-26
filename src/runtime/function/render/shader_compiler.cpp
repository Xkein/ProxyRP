#include "shader_compiler.h"
#include "core/log/log_system.h"
#include "platform/file/file_manager.h"

#include <ShaderConductor/ShaderConductor.hpp>
#include <filesystem>
#include <vector>
#include <string>

using ShaderConductor::Compiler;
using ShaderConductor::ShaderStage;
using ShaderConductor::ShadingLanguage;

tf::Executor GExecutor;

ShaderStage GetShaderStage(EShaderFrequency frequency) {
    switch (frequency)
    {
        case SF_Vertex:
            return ShaderStage::VertexShader;
        case SF_Pixel:
            return ShaderStage::PixelShader;
        case SF_Geometry:
            return ShaderStage::GeometryShader;
        case SF_Compute:
            return ShaderStage::ComputeShader;
    }

    return ShaderStage::NumShaderStages;
}

tf::Future<std::optional<ShaderCompiledInfo>> ShaderCompiler::BeginCompileShader(ShaderType*                shader_type,
                                                                                 std::shared_ptr<ShaderMap> shader_map)
{
    return GExecutor.async(Compile, shader_type, shader_map);
}

tf::Future<void> ShaderCompiler::BeginCompileShaders(std::vector<ShaderType*>   shader_types,
                                                     std::shared_ptr<ShaderMap> shader_map)
{
    tf::Taskflow taskflow;

    for (ShaderType* shader_type : shader_types)
    {
        taskflow.emplace([shader_type, shader_map]() { Compile(shader_type, std::move(shader_map)); });
    }

    return GExecutor.run(taskflow);
}

ShaderCompiledInfo ShaderCompiler::Compile(ShaderType* shader_type, std::shared_ptr<ShaderMap> shader_map)
{
    if (!shader_type)
    {
        LOG_ERROR("ShaderCompiler::Compile: empty ShaderType!");
        return ShaderCompiledInfo
        {
            .HasError = true,
        };
    }

    String shader_source_code = FileManager::ReadString(shader_type->SourceFile);

    Compiler::Options options {
        .enableDebugInfo = true,
    };

    Compiler::SourceDesc source_desc {
        .source     = shader_source_code.c_str(),
        .fileName   = shader_type->SourceFile.c_str(),
        .entryPoint = shader_type->EnterPoint.c_str(),
        .stage      = GetShaderStage(shader_type->Frequency),
    };

    Compiler::TargetDesc target_desc {
        .language = ShadingLanguage::SpirV
    };

    Compiler::ResultDesc result = Compiler::Compile(source_desc, options, target_desc);

    std::vector<byte> shader_data(result.target.Size());
    memcpy(shader_data.data(), result.target.Data(), result.target.Size());

    ShaderCompiledInfo compiled_info {
        .ShaderData = std::move(shader_data),
        .Message    = String((const char*)result.errorWarningMsg.Data(), result.errorWarningMsg.Size()),
        .HasError   = result.hasError,
    };

    return compiled_info;
}
