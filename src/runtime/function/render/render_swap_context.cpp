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

void RenderSwapContext::SwapLogicRenderData()
{
    if (!IsReadyToSwap())
        return;

    Swap();
}

bool RenderSwapContext::IsReadyToSwap() const
{
    return false;
}

void RenderSwapContext::Swap()
{


    std::swap(LogicSwapDataIndex, RenderSwapDataIndex);
}
