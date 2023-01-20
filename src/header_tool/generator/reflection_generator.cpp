#include "reflection_generator.h"
#include "language_types/class.h"
#include "template/template_manager.h"
#include "meta/meta_utils.h"

#include <filesystem>
#include <set>

ReflectionGenerator::ReflectionGenerator(std::string                             source_directory,
                                         std::function<std::string(const std::string&)> get_include_function) :
    Generator(source_directory + "/_generated/reflection", source_directory, get_include_function)
{
    PrepareStatus(OutPath);
}

int ReflectionGenerator::Generate(const std::string& path, SchemaModule schema)
{
    std::string file_path = ProcessFileName(path);
    Mustache::data mustache_data;
    Mustache::data include_headfiles(Mustache::data::type::list);
    Mustache::data class_defines(Mustache::data::type::list);
    
    include_headfiles.push_back(Mustache::data("headfile_name", utils::get_relative_path(RootPath, path)));

    std::set<std::string> class_names;

    for (auto& class_temp : schema.Classes)
    {
        if (!class_temp->ShouldCompile())
            continue;

        class_names.insert(class_temp->GetClassName());

        Mustache::data class_def;
        GenClassRenderData(class_temp, class_def);

        for (auto& field : class_temp->Fields)
        {
            if (!field->ShouldCompile())
                continue;

            
        }


        class_defines.push_back(class_def);
    }

    mustache_data.set("class_defines", class_defines);
    mustache_data.set("include_headfiles", include_headfiles);

    std::string render_string = GTemplateManager->RenderByTemplate("common_reflection.h", mustache_data);
    utils::save_file(render_string, file_path);

    for (auto& class_name : class_names)
    {
        TypeList.push_back(Mustache::data("class_name", class_name));
    }

    HeadFileList.push_back(Mustache::data("headfile_name", utils::get_relative_path(RootPath, file_path)));

    return 0;
}

void ReflectionGenerator::Finish()
{
    Mustache::data mustache_data;
    mustache_data.set("include_headfiles", HeadFileList);
    mustache_data.set("include_cpp_files", CppFileList);
    mustache_data.set("class_defines", TypeList);

    std::string render_string = GTemplateManager->RenderByTemplate("all_reflection.h", mustache_data);
    utils::save_file(render_string, OutPath + "/all_reflection.h");
}

ReflectionGenerator::~ReflectionGenerator() {}

void ReflectionGenerator::PrepareStatus(const std::string& path)
{
    Generator::PrepareStatus(path);

    GTemplateManager->LoadTemplate(RootPath, "all_reflection.h");
    GTemplateManager->LoadTemplate(RootPath, "common_reflection.h");
}

std::string ReflectionGenerator::ProcessFileName(const std::string& path)
{
    std::filesystem::path out_path =
        OutPath / std::filesystem::path(path).filename().replace_extension("reflection.gen.h");
    return out_path.string();
}
