#pragma once

#include <mustache.hpp>

namespace Mustache = kainjow::mustache;

#include <string>
#include <memory>

class TemplateManager
{
public:
    void LoadTemplate(std::string path, std::string template_name);

    std::string RenderByTemplate(std::string template_name, Mustache::data template_data);

private:
    std::unordered_map<std::string, std::string> TemplatePool;
};

extern std::shared_ptr<TemplateManager> GTemplateManager;
