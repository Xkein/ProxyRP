#pragma once

#include "function/render/shader.h"

#include <map>
#include <memory>

class ShaderMap
{
public:
    Shader* GetShader(ShaderType* shader_type);

    bool HasShader(ShaderType* shader_type);

    void AddShader(Shader* shader);

private:

    std::map<StringView, std::shared_ptr<Shader>> ShaderMapContent;
};

