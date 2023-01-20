#include "serializer_generator.h"
#include "language_types/class.h"
#include "template/template_manager.h"
#include "meta/meta_utils.h"

#include <filesystem>

SerializerGenerator::SerializerGenerator(std::string                             source_directory,
                                         std::function<std::string(const std::string&)> get_include_function) :
    Generator(source_directory + "/_generated/serializer", source_directory, get_include_function)
{
    PrepareStatus(OutPath);
}

int SerializerGenerator::Generate(const std::string& path, SchemaModule schema)
{
    std::string file_path = ProcessFileName(path);

    Mustache::data mustache_data;
    Mustache::data include_headfiles(Mustache::data::type::list);
    Mustache::data class_defines(Mustache::data::type::list);

    include_headfiles.push_back(Mustache::data("headfile_name", utils::get_relative_path(RootPath, path)));
    for (auto& class_temp : schema.Classes)
    {
        if (!class_temp->ShouldCompileFields())
            continue;

        Mustache::data class_def;
        GenClassRenderData(class_temp, class_def);

        auto AddHeader = [&](const std::string& name) {
            auto include_file = GetIncludeFunc(name);
            if (include_file.empty())
                return;
            auto include_file_base = ProcessFileName(include_file);
            if (include_file_base != file_path)
            {
                include_headfiles.push_back(
                    Mustache::data("headfile_name", utils::get_relative_path(RootPath, include_file_base)));
            }
        };
        for (size_t i = 0; i < class_temp->BaseClasses.size(); i++)
        {
            AddHeader(class_temp->BaseClasses[i]->Name);
        }

        for (auto& field : class_temp->Fields)
        {
            if (!field->ShouldCompile())
                continue;
            if (field->Type.find("std::vector") == 0)
            {
                AddHeader(field->Name);
            }
        }

        class_defines.push_back(class_def);
        ClassDefines.push_back(class_def);
    }

    mustache_data.set("class_defines", class_defines);
    mustache_data.set("include_headfiles", include_headfiles);

    std::string cpp_file_path = utils::change_extension(file_path, ".cpp");
    mustache_data.set("my_header_file", utils::get_filename(file_path));

    std::string render_string = GTemplateManager->RenderByTemplate("common_serializer.h", mustache_data);
    utils::save_file(render_string, file_path);

    //render_string = GTemplateManager->RenderByTemplate("common_serializer.cpp", mustache_data);
    //utils::save_file(render_string, cpp_file_path);

    IncludeHeadFiles.push_back(Mustache::data("headfile_name", utils::get_relative_path(RootPath, file_path)));
    IncludeCppFiles.push_back(Mustache::data("cpp_file_name", utils::get_relative_path(RootPath, cpp_file_path)));

    return 0;
}

void SerializerGenerator::Finish()
{
    Mustache::data mustache_data;
    mustache_data.set("class_defines", ClassDefines);
    mustache_data.set("include_headfiles", IncludeHeadFiles);
    mustache_data.set("include_cpp_files", IncludeCppFiles);

    std::string render_string = GTemplateManager->RenderByTemplate("all_serializer.h", mustache_data);
    utils::save_file(render_string, OutPath + "/all_serializer.h");

    render_string = GTemplateManager->RenderByTemplate("all_serializer.ipp", mustache_data);
    utils::save_file(render_string, OutPath + "/all_serializer.ipp");
}

SerializerGenerator::~SerializerGenerator() {}

void SerializerGenerator::PrepareStatus(const std::string& path)
{
    Generator::PrepareStatus(path);
    
    GTemplateManager->LoadTemplate(RootPath, "all_serializer.ipp");
    GTemplateManager->LoadTemplate(RootPath, "all_serializer.h");
    GTemplateManager->LoadTemplate(RootPath, "common_serializer.h");
}

std::string SerializerGenerator::ProcessFileName(const std::string& path)
{
    std::filesystem::path out_path =
        OutPath / std::filesystem::path(path).filename().replace_extension("serializer.gen.h");
    return out_path.string();
}
