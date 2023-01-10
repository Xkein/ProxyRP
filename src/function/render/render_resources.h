#pragma once

class RenderResource
{
public:
    virtual void InitRHI() {}

    virtual void ReleaseRHI() {}
};
