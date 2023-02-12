#include "proxy_component.h"
#include "function/framework/object/game_object.h"
#include "function/framework/component/transform/transform_component.h"

void ProxyComponent::Tick(float delta_time) {}

void ProxyComponent::UpdateTransform(Transformf transform)
{
    std::shared_ptr<GameObject> game_object = ParentObject.lock();
    TransformComponent*         transform_component = game_object->GetComponent<TransformComponent>();

    if (!transform_component)
        return;

    transform_component->SetTransform(transform);
}
