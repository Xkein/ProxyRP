#pragma once

#include <vector>
#include <string>
#include <memory>

class Class;
class Enum;

struct SchemaModule
{
    std::string Name;

    std::vector<std::shared_ptr<Class>> Classes;
    std::vector<std::shared_ptr<Enum>>  Enumerations;
};
