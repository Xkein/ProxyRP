#include "parser.h"
#include "cursor/cursor.h"
#include "language_types/class.h"
#include "language_types/enum.h"
#include "generator/reflection_generator.h"
#include "generator/serializer_generator.h"
#include "meta/meta_utils.h"

#include <filesystem>
#include <fstream>

void MetaParser::Prepare() {}

MetaParser::MetaParser(const std::string& project_input_file,
                       const std::string& include_path,
                       const std::string& sys_include,
                       const std::string& module_name,
                       bool               show_errors) :
    ProjectInputFile(project_input_file),
    SysInclude(sys_include),
    ModuleName(module_name), IsShowErrors(show_errors)
{
    
    WorkPaths = utils::split(utils::load_file(include_path), ";");

    SourceIncludeFileName = (std::filesystem::path(project_input_file).parent_path() / "parser_header.h").string();

    Generators.emplace_back(
        new SerializerGenerator(WorkPaths[0], std::bind(&MetaParser::GetIncludeFile, this, std::placeholders::_1)));
    Generators.emplace_back(
        new ReflectionGenerator(WorkPaths[0], std::bind(&MetaParser::GetIncludeFile, this, std::placeholders::_1)));
}

MetaParser::~MetaParser() {
    Generators.clear();

    if (TranslationUnit)
        clang_disposeTranslationUnit(TranslationUnit);
    if (Index)
        clang_disposeIndex(Index);
}

int MetaParser::Parse()
{
    bool parsed = ParseProject();
    if (!parsed)
    {
        std::cerr << "Parsing project file error!" << std::endl;
        return -1;
    }

    std::cout << "Parsing whole project..." << std::endl;

    Index = clang_createIndex(true, IsShowErrors);

    std::string pre_include = "-I";
    std::string sys_include;
    if (SysInclude != "*")
    {
        sys_include = pre_include + SysInclude;
        arguments.push_back(sys_include.c_str());
    }

    auto paths = WorkPaths;
    for (size_t i = 0; i < paths.size(); i++)
    {
        paths[i] = pre_include + utils::trim(paths[i], " \t\r\n");
        arguments.push_back(paths[i].c_str());
    }

    std::filesystem::path input_path(SourceIncludeFileName);
    if (!std::filesystem::exists(input_path))
    {
        std::cerr << input_path << " is not exist" << std::endl;
        return -2;
    }

    TranslationUnit = clang_createTranslationUnitFromSourceFile(
        Index, SourceIncludeFileName.c_str(), arguments.size(), arguments.data(), 0, nullptr);

    auto cursor = clang_getTranslationUnitCursor(TranslationUnit);

    Namespace temp_namespace;

    BuildClassAST(cursor, temp_namespace);

    temp_namespace.clear();

    return 0;
}

void MetaParser::GenerateFiles()
{
    std::cout << "Start generate runtime schemas(" << SchemaModules.size() << ")..." << std::endl;
    for (auto& generator : Generators)
    {
        for (auto& [path, schema] : SchemaModules)
        {
            generator->Generate(path, schema);
        }
    }

    Finish();
}

void MetaParser::Finish()
{
    for (auto& generator : Generators)
    {
        generator->Finish();
    }
}

bool MetaParser::ParseProject()
{
    bool result = true;
    std::cout << "Parsing project file: " << ProjectInputFile << std::endl;

    std::string context = utils::load_file(ProjectInputFile);

    auto         inlcude_files = utils::split(context, ";");
    std::fstream include_file;

    include_file.open(SourceIncludeFileName, std::ios::out);
    if (!include_file.is_open())
    {
        std::cout << "Could not open the Source Include file: " << SourceIncludeFileName << std::endl;
        return false;
    }

    std::cout << "Generating the Source Include file: " << SourceIncludeFileName << std::endl;

    std::string output_filename = std::filesystem::path(SourceIncludeFileName).filename().string();

    if (output_filename.empty())
    {
        output_filename = "META_INPUT_HEADER_H";
    }
    else
    {
        utils::replace_all(output_filename, ".", "_");
        utils::replace_all(output_filename, " ", "_");
        std::transform(output_filename.begin(), output_filename.end(), output_filename.begin(), toupper);
    }
    include_file << "#ifndef __" << output_filename << "__" << std::endl;
    include_file << "#define __" << output_filename << "__" << std::endl;

    for (auto include_item : inlcude_files)
    {
        std::string temp_string(include_item);
        utils::replace_all(temp_string, "\\", "/");
        temp_string = utils::trim(temp_string, " \t\r\n");
        include_file << "#include  \"" << temp_string << "\"" << std::endl;
    }

    include_file << "#endif" << std::endl;
    include_file.close();
    return result;
}

void MetaParser::BuildClassAST(const Cursor& cursor, Namespace& current_namespace)
{
    for (auto& child : cursor.GetChildren())
    {
        CXCursorKind kind = child.GetKind();

        if (child.IsDefinition())
        {
            if (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl)
            {
                auto class_ptr = std::make_shared<Class>(child, current_namespace);

                // std::cout << class_ptr->Name << std::endl;

                if (class_ptr->ShouldCompile())
                {
                    auto file = class_ptr->GetSourceFile();
                    SchemaModules[file].Classes.emplace_back(class_ptr);
                    TypeTable[class_ptr->DisplayName] = file;
                }
            }
            else if (kind == CXCursor_EnumDecl)
            {
                auto enum_ptr = std::make_shared<Enum>(child, current_namespace);

                if (enum_ptr->ShouldCompile())
                {
                    auto file = enum_ptr->GetSourceFile();
                    SchemaModules[file].Enumerations.emplace_back(enum_ptr);
                    TypeTable[enum_ptr->Name] = file;
                }
            }
        }
        else
        {
            if (kind == CXCursor_Namespace)
            {
                auto display_name = child.GetDisplayName();
                if (!display_name.empty())
                {
                    current_namespace.emplace_back(display_name);
                    BuildClassAST(child, current_namespace);
                    current_namespace.pop_back();
                }
            }
        }
    }
}

std::string MetaParser::GetIncludeFile(const std::string& name)
{
    auto iter = TypeTable.find(name);
    return iter == TypeTable.end() ? std::string() : iter->second;
}


