#include "proxy_manager_component.h"
#include "proxy_manager.h"

void ProxyManagerComponent::PostLoadResource(std::weak_ptr<GameObject> parent_object)
{
    Component::PostLoadResource(parent_object);

    Manager = std::make_shared<ProxyManager>();
}

void ProxyManagerComponent::Tick(float delta_time)
{
    if (!Initialized)
    {
        ProxyManagerInitInfo init_info;
        init_info.Address = Address;
        init_info.Port    = Port;
        Manager->Initialize(init_info);
        Initialized = true;
    }

    Manager->Tick(delta_time);
}
