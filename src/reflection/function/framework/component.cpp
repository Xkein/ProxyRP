
#include "function/framework/component.h"

#include <rttr/registration>

using namespace rttr;

RTTR_REGISTRATION
{
	registration::class_<Component>("Component")
		.constructor<>()
		.method("Tick", &Component::Tick);
}