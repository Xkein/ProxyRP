#include "shader_map.h"

#include <mutex>

std::mutex ShaderMapMutex;

Shader* ShaderMap::GetShader(ShaderType* shader_type)
{
    auto iter = ShaderMapContent.find(shader_type->GetHashedName());
    if (iter == ShaderMapContent.end())
        return nullptr;

    Shader* shader = iter->second.get();
    if (shader->Index < 0)
        return nullptr;

    return shader;
}

bool ShaderMap::HasShader(ShaderType* shader_type)
{
    return GetShader(shader_type) != nullptr;
}

void ShaderMap::AddShader(Shader* shader) {
    std::lock_guard<std::mutex> guard(ShaderMapMutex);

    ShaderMapContent[shader->Type->GetHashedName()] = std::shared_ptr<Shader>(shader);
}
