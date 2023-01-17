#pragma once

#include <cstdint>

struct RenderSwapData
{

};

class RenderSwapContext
{
    RenderSwapData& GetLogicSwapData();
    RenderSwapData& GetRenderSwapData();

    void SwapLogicRenderData();

private:
    uint8_t LogicSwapDataIndex;
    uint8_t RenderSwapDataIndex;

    RenderSwapData SwapData[2];

    bool IsReadyToSwap() const;
    void Swap();
};

