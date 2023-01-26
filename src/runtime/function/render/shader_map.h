#pragma once

#include "function/render/shader.h"
#include "resource/asset/asset_registry.h"

class ShaderMap
{
public:
    Shader* GetShader(ShaderType* shader_type);

    bool HasShader(ShaderType* shader_type);


    void LoadCachedShader(const String& cached_shader_path);


private:

    AssetRegistry ShaderRegistry;
};
