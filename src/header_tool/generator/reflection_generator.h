#pragma once

#include "generator.h"

class ReflectionGenerator : public Generator
{
public:
    ReflectionGenerator(std::string source_directory, std::function<std::string(const std::string&)> get_include_function);

    virtual int Generate(const std::string& path, SchemaModule schema) override;

    virtual void Finish() override;

    virtual ~ReflectionGenerator() override;

protected:
    virtual void PrepareStatus(const std::string& path) override;

    virtual std::string ProcessFileName(const std::string& path) override;

private:
    Mustache::data HeadFileList {Mustache::data::type::list};
    Mustache::data CppFileList {Mustache::data::type::list};
    Mustache::data TypeList {Mustache::data::type::list};
    Mustache::data EnumList {Mustache::data::type::list};
};
