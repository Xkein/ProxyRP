#pragma once

#include "platform/string.h"
#include "function/render/render_object.h"
#include "resource/res_type/global/global_rendering.h"

#include <cstdint>
#include <optional>

struct LevelIBLResourceDesc
{
    SkyBoxIrradianceMap SkyboxIrradianceMap;
    SkyBoxSpecularMap   SkyboxSpecularMap;
    String              BrdfMap;
};

struct LevelColorGradingResourceDesc
{
    String ColorGradingMap;
};

struct LevelResourceDesc
{
    LevelIBLResourceDesc          IblResourceDesc;
    LevelColorGradingResourceDesc ColorGradingResourceDesc;
};

struct CameraSwapDataDesc
{
    std::optional<float>     FovX;
    std::optional<Matrix4x4> ViewMatrix;
};

struct GameObjectResourceDesc
{
    std::vector<GameObjectDesc> GameObjectDescs;

    size_t CurIndex {0};

    bool IsEmpty() const
    {
        return GameObjectDescs.empty() || CurIndex >= GameObjectDescs.size();
    }

    void Add(GameObjectDesc& desc)
    {
        GameObjectDescs.push_back(desc);
    }

    GameObjectDesc& PopAndGetNextProcessObject() {
        return GameObjectDescs[CurIndex++];
    }
};

struct RenderSwapData
{
    std::optional<LevelResourceDesc>      LevelResource;
    std::optional<GameObjectResourceDesc> GameObjectResource;
    std::optional<GameObjectResourceDesc> GameObjectToDelete;
    std::optional<CameraSwapDataDesc>     CameraSwapData;

    void AddDirtyGameObject(GameObjectDesc& desc);
    void AddDeleteGameObject(GameObjectDesc& desc);
};

class RenderSwapContext
{
public:
    RenderSwapData& GetLogicSwapData();
    RenderSwapData& GetRenderSwapData();
    void            ResetLevelRsourceSwapData();
    void            ResetGameObjectResourceSwapData();
    void            ResetGameObjectToDelete();
    void            ResetCameraSwapData();

    void SwapLogicRenderData();

private:
    uint8_t LogicSwapDataIndex {0};
    uint8_t RenderSwapDataIndex {1};

    RenderSwapData SwapData[2];

    bool IsReadyToSwap() const;
    void Swap();
};

