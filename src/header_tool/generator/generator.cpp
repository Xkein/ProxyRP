#include "generator.h"
#include "language_types/class.h"
#include "language_types/enum.h"

#include <filesystem>

void Generator::PrepareStatus(const std::string& path)
{
    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directories(path);
    }
}

void Generator::GenClassRenderData(std::shared_ptr<Class> class_temp, Mustache::data& class_def)
{
    class_def.set("class_name", class_temp->GetClassName());
    class_def.set("class_base_class_size", std::to_string(class_temp->BaseClasses.size()));
    class_def.set("class_need_register", true);
    class_def.set("class_is_abstract", class_temp->IsAbstract);

    if (class_temp->BaseClasses.size() > 0)
    {
        Mustache::data class_base_class_defines(Mustache::data::type::list);
        class_def.set("class_has_base", true);
        for (size_t i = 0; i < class_temp->BaseClasses.size(); i++)
        {
            Mustache::data class_base_class_def;
            class_base_class_def.set("class_base_class_name", class_temp->BaseClasses[i]->Name);
            class_base_class_def.set("class_base_class_index", std::to_string(i));
            class_base_class_defines.push_back(class_base_class_def);
        }
        class_def.set("class_base_class_defines", class_base_class_defines);
    }

    Mustache::data class_field_defines(Mustache::data::type::list);
    GenClassFieldRenderData(class_temp, class_field_defines);
    class_def.set("class_field_defines", class_field_defines);

    Mustache::data class_method_defines(Mustache::data::type::list);
    GenClassMethodRenderData(class_temp, class_method_defines);
    class_def.set("class_method_defines", class_method_defines);
}

void Generator::GenClassFieldRenderData(std::shared_ptr<Class> class_temp, Mustache::data& field_defs)
{
    for (auto& field : class_temp->Fields)
    {
        if (!field->ShouldCompile())
            continue;

        Mustache::data field_define;

        field_define.set("class_field_name", field->Name);
        field_define.set("class_field_type", field->Type);
        field_define.set("class_field_display_name", field->DisplayName);
        bool is_vector = field->Type.find("std::vector<") == 0;
        field_define.set("class_field_is_vector", is_vector);

        field_defs.push_back(field_define);
    }

}

void Generator::GenClassMethodRenderData(std::shared_ptr<Class> class_temp, Mustache::data& method_defs)
{
    for (auto& method : class_temp->Methods)
    {
        if (!method->ShouldCompile())
            continue;

        Mustache::data method_define;

        method_define.set("class_method_name", method->Name);

        method_defs.push_back(method_define);
    }
}

void Generator::GenEnumRenderData(std::shared_ptr<Enum> enum_temp, Mustache::data& enum_defs)
{
    enum_defs.set("enum_name", enum_temp->Name);
    enum_defs.set("enum_is_scoped", enum_temp->IsEnumClass);

    Mustache::data enum_const_defines(Mustache::data::type::list);
    for (auto& enum_const : enum_temp->Constants)
    {
        if (!enum_const->ShouldCompile())
            continue;

        Mustache::data enum_const_define;

        enum_const_define.set("enum_const_name", enum_const->Name);
        enum_const_define.set("enum_const_display_name", enum_const->DisplayName);

        enum_const_defines.push_back(enum_const_define);
    }

    enum_defs.set("enum_const_defines", enum_const_defines);
}
