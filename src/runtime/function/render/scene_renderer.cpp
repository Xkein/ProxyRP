#include "scene_renderer.h"
#include "function/render/render_camera.h"
#include "function/render/render_scene.h"
#include "function/render/render_pipeline.h"

void SceneRenderer::PrepareData()
{
    RenderPipelinePrepareInfo pipeline_prepare_info;
    pipeline_prepare_info.Scene           = Scene;
    pipeline_prepare_info.ResourceManager = ResourceManager;
    Pipeline->PreparePassData(&pipeline_prepare_info);
}

void SceneRenderer::UpdatePerBuffer(std::shared_ptr<RenderCamera> camera)
{
    Matrix4x4 view_matrix      = camera->GetViewMatrix();
    Matrix4x4 proj_matrix      = camera->GetPerspectiveMatrix();
    Vector3f  camera_position  = camera->Position;
    Matrix4x4 proj_view_matrix = proj_matrix * view_matrix;

    Vector3f ambient_light = Scene->Light.Ambient.Irradiance;
    uint32_t point_light_num = Scene->Light.PointList.Lights.size();

    Scene->PerframeStorageBufferObject.ProjViewMatrix = proj_view_matrix;
    Scene->PerframeStorageBufferObject.CameraPosition = camera_position;
    Scene->PerframeStorageBufferObject.AmbientLight   = ambient_light;
    Scene->PerframeStorageBufferObject.PointLightNum  = point_light_num;

    Scene->PointLightShadowPerframeStorageBufferObject.PointLightNum = point_light_num;
    for (uint32_t i = 0; i < point_light_num; i++)
    {
        auto point_light = Scene->Light.PointList.Lights[i];

        Vector3f point_light_intensity = point_light.GetIntensity();
        float radius = point_light.CalculateRadius();

        Scene->PerframeStorageBufferObject.PointLights[i].Position = point_light.Position;
        Scene->PerframeStorageBufferObject.PointLights[i].Radius   = radius;
        Scene->PerframeStorageBufferObject.PointLights[i].Intensity = point_light_intensity;

        Scene->PointLightShadowPerframeStorageBufferObject.PointLightsPositionAndRadius[i] = Vector4f(point_light.Position.x(), point_light.Position.y(), point_light.Position.z(), radius);
    }
    
    Scene->PerframeStorageBufferObject.DirectionalLight.Direction = Scene->Light.Directional.Direction.normalized();
    Scene->PerframeStorageBufferObject.DirectionalLight.Color     = Scene->Light.Directional.Color;
}

void SceneRenderer::InitializeUIRenderBackend(WindowUI* window_ui)
{
    Pipeline->InitializeUIRenderBackend(window_ui);
}
