#include "render_resource.h"

RenderResource::~RenderResource()
{
    // check release
}

void RenderResource::InitResource()
{
    InitRHI();
}

void RenderResource::ReleaseResource()
{
    ReleaseRHI();
}

void RenderResource::UpdateRHI()
{
    ReleaseRHI();
    InitRHI();
}
