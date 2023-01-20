#pragma once

#include "common/schema_module.h"

#include <string>
#include <functional>
#include <memory>
#include <mustache.hpp>

namespace Mustache = kainjow::mustache;

class Generator
{
public:
    Generator(std::string out_path, std::string root_path, std::function<std::string(const std::string&)> get_include_func) :
        OutPath(out_path), RootPath(root_path), GetIncludeFunc(get_include_func)
    {}

    virtual int Generate(const std::string& path, SchemaModule schema) = 0;

    virtual void Finish() = 0;
   
    virtual ~Generator() {}

protected:
    virtual void        PrepareStatus(const std::string& path);
    virtual void        GenClassRenderData(std::shared_ptr<Class> class_temp, Mustache::data& class_def);
    virtual void        GenClassFieldRenderData(std::shared_ptr<Class> class_temp, Mustache::data& field_defs);
    virtual std::string ProcessFileName(const std::string& path) = 0;

    std::string OutPath;
    std::string RootPath;

    std::function<std::string(const std::string&)> GetIncludeFunc;

};
