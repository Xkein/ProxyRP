#include "render_pass.h"

void RenderPass::SetCommonInfo(RenderPassCommonInfo* common_info)
{
    RHI        = common_info->RHI;
    PassCommon = common_info->PassCommon;
}
