#pragma once

#include <vector>
#include <string>
#include <memory>

class Class;

struct SchemaModule
{
    std::string Name;

    std::vector<std::shared_ptr<Class>> Classes;
};
