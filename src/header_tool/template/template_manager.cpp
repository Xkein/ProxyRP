#include "template_manager.h"
#include "meta/meta_utils.h"

void TemplateManager::LoadTemplate(std::string path, std::string template_name)
{
    TemplatePool[template_name] = utils::load_file(path + "/template/" + template_name + ".mustache");
}

std::string TemplateManager::RenderByTemplate(std::string template_name, Mustache::data template_data)
{
    if (!TemplatePool.contains(template_name))
        return "";

    Mustache::mustache tmpl(TemplatePool[template_name]);
    return tmpl.render(template_data);
}

std::shared_ptr<TemplateManager> GTemplateManager = std::make_shared<TemplateManager>();


