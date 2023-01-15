
#include "function/framework/component/component.h"

#include "core/reflection/reflection_register.h"

using namespace rttr;

RTTR_REGISTRATION
{
	registration::class_<Component>("Component")
		.constructor<>()
		.method("Tick", &Component::Tick);
}

