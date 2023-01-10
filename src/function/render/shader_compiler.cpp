
#include <ShaderConductor/ShaderConductor.hpp>
#include <filesystem>
#include <vector>
#include <string>

#include "shader_compiler.h"

using ShaderConductor::Compiler;

String LoadHlsl(const std::filesystem::path& hlsl_path) {
    return {};
}

//RHIShader* CompileShader(const char* path) {
//    std::filesystem::path hlsl_path;
//
//    String hlsl = LoadHlsl(hlsl_path);
//
//    Compiler::Options options;
//    Compiler::SourceDesc source_desc;
//    Compiler::TargetDesc target_desc;
//    Compiler::Compile(source_desc, options, target_desc);
//
//	RHIShader* shader;
//    return shader;
//}