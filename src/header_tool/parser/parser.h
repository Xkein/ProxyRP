#pragma once

#include "common/namespace.h"
#include "generator/generator.h"

#include <clang-c/Index.h>
#include <string>
#include <vector>

class Cursor;

class MetaParser
{
public:
    static void Prepare();

    MetaParser(const std::string& project_input_file,
               const std::string& include_path,
               const std::string& sys_include,
               const std::string& module_name,
               bool               show_errors);
    ~MetaParser();

    int Parse();

    void GenerateFiles();

private:
    bool ParseProject();
    void BuildClassAST(const Cursor& cursor, Namespace& current_namespace);

    void Finish();

    std::string GetIncludeFile(const std::string& name);

    std::string              ProjectInputFile;
    std::vector<std::string> WorkPaths;
    std::string              SysInclude;
    std::string              ModuleName;
    bool                     IsShowErrors;

    std::string SourceIncludeFileName;

    std::vector<std::shared_ptr<Generator>> Generators;

    CXIndex           Index;
    CXTranslationUnit TranslationUnit;

    std::unordered_map<std::string, std::string>  TypeTable;
    std::unordered_map<std::string, SchemaModule> SchemaModules;

    std::vector<const char*> arguments = {
        "-x",
        "c++",
        "-std=c++20",
        "-D__clang__",
        "-DNDEBUG",
        "-D__REFLECTION_HEADER_TOOL__",
        "-w",
        "-MG",
        "-M",
        "-ferror-limit=0",
        "-oclang_log.txt",
    };

};
