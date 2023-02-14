#include "camera_component.h"
#include "core/math/angle.h"
#include "core/math/math.h"
#include "core/log/log_system.h"
#include "function/global/global_context.h"
#include "function/render/render_system.h"
#include "function/framework/level/level.h"
#include "function/framework/object/game_object.h"
#include "function/framework/world/world_manager.h"
#include "function/input/input_system.h"
#include "resource/asset/asset_manager.h"
#include "resource/config/config_manager.h"
#include "resource/res_type/global/global_rendering.h"

void CameraComponent::PostLoadResource(std::weak_ptr<GameObject> parent_object)
{
    Component::PostLoadResource(parent_object);

    const StringView camera_type_name = Parameter.GetTypeName();
    if (camera_type_name == "FreeCameraParameter")
    {
        Mode = CameraMode::Free;
    }
    else
    {
        LOG_ERROR("invalid camera type: {}", camera_type_name);
        return;
    }

    if (Parameter->UseGlobalCameraConfig)
    {
        GlobalRenderingResource global_rendering_res;
        GAssetManager->LoadAsset(GConfigManager->Global.GlobalRenderingResourceUrl, global_rendering_res);
        const auto& camera_pose = global_rendering_res.CameraConfig.Pose;

        Position = camera_pose.Position;
        Forward  = camera_pose.Target - camera_pose.Position;
        Left     = camera_pose.Up.cross(Forward);
        Up       = Forward.cross(Left);
    }

    //RenderSwapContext& render_swap_context = GRenderSystem->GetSwapContext();
    //RenderSwapData&    logic_swap_data     = render_swap_context.GetLogicSwapData();

    //CameraSwapDataDesc camera_swap_data;
    //camera_swap_data.FovX          = Parameter->Fov;
    //logic_swap_data.CameraSwapData = camera_swap_data;
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
    GameCommand command = GInputSystem->GetGameCommand();
    if (!command)
        return;

    Quaternionf q_yaw(AngleAxisf(GInputSystem->CursorDeltaYaw, Vector3f::UnitZ()));
    Quaternionf q_pitch(AngleAxisf(GInputSystem->CursorDeltaPitch, Left));
    
    Forward = q_yaw * q_pitch * Forward;
    Left    = q_yaw * q_pitch * Left;
    Up      = Forward.cross(Left);

    Vector3f move_direction = Vector3f::Zero();

    if (command.Forward)
    {
        move_direction += Forward;
    }
    if (command.Backward)
    {
        move_direction -= Forward;
    }
    if (command.Left)
    {
        move_direction += Left;
    }
    if (command.Right)
    {
        move_direction -= Left;
    }
    if (command.Fly)
    {
        move_direction += Vector3f::UnitZ();
    }
    if (command.Fall)
    {
        move_direction -= Vector3f::UnitZ();
    }

    Position += move_direction * 2.f * delta_time;

    Matrix4x4 desired_mat = LookAt(Position, Position + Forward, Up);

    RenderSwapContext& render_swap_context = GRenderSystem->GetSwapContext();
    RenderSwapData&    logic_swap_data     = render_swap_context.GetLogicSwapData();

    CameraSwapDataDesc camera_swap_data;
    camera_swap_data.ViewMatrix    = desired_mat;
    logic_swap_data.CameraSwapData = camera_swap_data;
}
