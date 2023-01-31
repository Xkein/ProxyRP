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

    
}

void SceneRenderer::InitializeUIRenderBackend(WindowUI* window_ui)
{
    Pipeline->InitializeUIRenderBackend(window_ui);
}
