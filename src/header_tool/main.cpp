
#include "parser/parser.h"

#include <chrono>
#include <iostream>

int Parse(std::string project_input_file,
          std::string include_path,
          std::string sys_include,
          std::string module_name,
          std::string show_errors);

int main(int argc, char** argv) {
    auto start_time = std::chrono::system_clock::now();

    const auto& project_input_file = argv[1];
    const auto& include_path       = argv[2];
    const auto& sys_include        = argv[3];
    const auto& module_name        = argv[4];
    const auto& show_errors        = argv[5];

    if (project_input_file&& include_path && sys_include && module_name && show_errors)
    {
        if (std::string(show_errors) == "0114514")
        {
            getchar();
        }

        int result = Parse(project_input_file, include_path, sys_include, module_name, show_errors);

        auto elapsed_time = std::chrono::system_clock::now() - start_time;
        std::cout << "Completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count()
                  << "ms" << std::endl;
        return result;
    }
    else
    {
        std::cerr << "Arguments parse error!" << std::endl;
        return -1;
    }


    return 0;
}

int Parse(std::string project_input_file,
          std::string include_path,
          std::string sys_include,
          std::string module_name,
          std::string show_errors)
{
    MetaParser::Prepare();

    MetaParser parser(project_input_file, include_path, sys_include, module_name, show_errors[0] != '0');

    int result = parser.Parse();
    if (result)
    {
        return result;
    }

    parser.GenerateFiles();

    return 0;
}
