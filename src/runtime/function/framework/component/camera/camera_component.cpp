#include "camera_component.h"
#include "core/log/log_system.h"
#include "function/global/global_context.h"
#include "function/framework/level/level.h"
#include "function/framework/object/game_object.h"
#include "function/framework/world/world_manager.h"

void CameraComponent::PostLoadResource(std::weak_ptr<GameObject> parent_object)
{
    Component::PostLoadResource(parent_object);

}
void CameraComponent::Tick(float delta_time)
{
    std::shared_ptr<GameObject> parent_object = ParentObject.lock();
    if (!parent_object)
        return;

    std::shared_ptr<Level> current_level = GWorldManager->GetCurrentLevel().lock();

    // check character

    switch (Mode)
    {
        case CameraMode::FirstPerson:
            TickFirstPersonCamera(delta_time);
            break;
        case CameraMode::ThirdPerson:
            TickThirdPersonCamera(delta_time);
            break;
        case CameraMode::Free:
            TickFreePersonCamera(delta_time);
            break;
        default:
            break;
    }

}
void CameraComponent::TickFirstPersonCamera(float delta_time)
{
    LOG_ERROR("TickFirstPersonCamera not implement");
}

void CameraComponent::TickThirdPersonCamera(float delta_time)
{
    LOG_ERROR("TickThirdPersonCamera not implement");
}

void CameraComponent::TickFreePersonCamera(float delta_time)
{

}
