#pragma once

#include "generator.h"

class SerializerGenerator : public Generator
{
public:
    SerializerGenerator(std::string source_directory, std::function<std::string(const std::string&)> get_include_function);

    virtual int Generate(const std::string& path, SchemaModule schema) override;

    virtual void Finish() override;

    virtual ~SerializerGenerator() override;

protected:
    virtual void PrepareStatus(const std::string& path) override;

    virtual std::string ProcessFileName(const std::string& path) override;

private:
    Mustache::data ClassDefines {Mustache::data::type::list};
    Mustache::data EnumDefines {Mustache::data::type::list};
    Mustache::data IncludeHeadFiles {Mustache::data::type::list};
    Mustache::data IncludeCppFiles {Mustache::data::type::list};
};
