
#include "function/framework/object/game_object.h"

#include "core/reflection/reflection_register.h"

using namespace rttr;

RTTR_REGISTRATION
{
    registration::class_<GameObject>("GameObject")
        .method("Tick", &GameObject::Tick);
}

