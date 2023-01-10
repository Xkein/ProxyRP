
#include "function/framework/game_object.h"

#include <rttr/registration>

using namespace rttr;

RTTR_REGISTRATION
{
    registration::class_<GameObject>("GameObject")
        .method("Tick", &GameObject::Tick);
}

