#include "shader_compiler.h"
#include "core/log/log_system.h"
#include "platform/file/file_manager.h"
#include "resource/config/config_manager.h"
#include "function/global/global_context.h"

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
        //Compile(shader_type, shader_map);
        taskflow.emplace([shader_type, shader_map]() {
            ShaderCompiledInfo compiled_info = Compile(shader_type, shader_map);
            SaveShader(shader_type, compiled_info);
        });
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

    LOG_INFO("Compiling shader {}, EntryPoint: {}, SourceFile: {}", shader_type->Name, shader_type->EntryPoint, shader_type->SourceFile);

    String shader_source_filename = (GConfigManager->ShaderPath / shader_type->SourceFile).string();
    String shader_source_code     = FileManager::ReadString(shader_source_filename);

    Compiler::Options options {
        .enableDebugInfo = true,
    };

    Compiler::SourceDesc source_desc {
        .source     = shader_source_code.c_str(),
        .fileName   = shader_source_filename.c_str(),
        .entryPoint = shader_type->EntryPoint.c_str(),
        .stage      = GetShaderStage(shader_type->Frequency),
        .loadIncludeCallback =
            [](const Char* include_name) {
                String            filename = (GConfigManager->ShaderPath / include_name).string();
                std::vector<byte> data     = FileManager::Read(filename);
                return ShaderConductor::Blob(data.data(), data.size());
            },
    };

    Compiler::TargetDesc target_desc {
        .language = ShadingLanguage::SpirV
    };

    ShaderCompiledInfo compiled_info;
    try
    {
        Compiler::ResultDesc result = Compiler::Compile(source_desc, options, target_desc);
        std::vector<byte>    shader_data(result.target.Size());
        memcpy(shader_data.data(), result.target.Data(), result.target.Size());

        compiled_info.ShaderData = std::move(shader_data);
        compiled_info.Message    = String((const char*)result.errorWarningMsg.Data(), result.errorWarningMsg.Size());
        compiled_info.HasError   = result.hasError;
    }
    catch (const std::exception& e)
    {
        compiled_info.Message  = e.what();
        compiled_info.HasError = true;
    }

    if (compiled_info.HasError)
    {
        LOG_ERROR("Shader {} Compile error, Message: {}", shader_type->SourceFile, compiled_info.Message);
    }
    else
    {
        LOG_INFO("Shader {} Compile finished, Message: {}", shader_type->Name, compiled_info.Message);
    }

    return compiled_info;
}

void ShaderCompiler::SaveShader(ShaderType* shader_type, ShaderCompiledInfo compiled_info)
{
    if (compiled_info.HasError)
        return;

    FileManager::Write(shader_type->GetCachedFilePath(), compiled_info.ShaderData);
}

tf::Future<void> __CompileAllShaders()
{
    LOG_INFO("Begin compiling all shaders.");
    return ShaderCompiler::BeginCompileShaders(ShaderType::GetAllShaderTypes(), nullptr);
}
