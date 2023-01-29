#include "render_swap_context.h"

#include <utility>

RenderSwapData& RenderSwapContext::GetLogicSwapData()
{
    return SwapData[LogicSwapDataIndex];
}

RenderSwapData& RenderSwapContext::GetRenderSwapData()
{
    return SwapData[RenderSwapDataIndex];
}

void RenderSwapContext::ResetLevelRsourceSwapData()
{
    SwapData[RenderSwapDataIndex].LevelResource.reset();
}

void RenderSwapContext::ResetGameObjectResourceSwapData()
{
    SwapData[RenderSwapDataIndex].GameObjectResource.reset();
}

void RenderSwapContext::ResetGameObjectToDelete()
{
    SwapData[RenderSwapDataIndex].GameObjectToDelete.reset();
}

void RenderSwapContext::ResetCameraSwapData()
{
    SwapData[RenderSwapDataIndex].CameraSwapData.reset();
}

void RenderSwapContext::SwapLogicRenderData()
{
    if (!IsReadyToSwap())
        return;

    Swap();
}

bool RenderSwapContext::IsReadyToSwap() const
{
    return !(SwapData[RenderSwapDataIndex].LevelResource.has_value() ||
             SwapData[RenderSwapDataIndex].GameObjectResource.has_value() ||
             SwapData[RenderSwapDataIndex].GameObjectToDelete.has_value() ||
             SwapData[RenderSwapDataIndex].CameraSwapData.has_value());
}

void RenderSwapContext::Swap()
{
    ResetLevelRsourceSwapData();
    ResetGameObjectResourceSwapData();
    ResetGameObjectToDelete();
    ResetCameraSwapData();

    std::swap(LogicSwapDataIndex, RenderSwapDataIndex);
}

void RenderSwapData::AddDirtyGameObject(GameObjectDesc& desc)
{
    if (GameObjectResource.has_value())
    {
        GameObjectResource->Add(desc);
    }
    else
    {
        GameObjectResourceDesc go_descs;
        go_descs.Add(desc);
        GameObjectResource = go_descs;
    }
}

void RenderSwapData::AddDeleteGameObject(GameObjectDesc& desc)
{
    if (GameObjectToDelete.has_value())
    {
        GameObjectToDelete->Add(desc);
    }
    else
    {
        GameObjectResourceDesc go_descs;
        go_descs.Add(desc);
        GameObjectToDelete = go_descs;
    }
}
