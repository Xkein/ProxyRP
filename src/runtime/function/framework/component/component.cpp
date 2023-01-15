#include "component.h"

void Component::PostLoadResource(std::weak_ptr<GameObject> parent_object)
{
    ParentObject = parent_object;
}
