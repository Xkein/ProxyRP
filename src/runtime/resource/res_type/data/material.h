#pragma

#include "core/reflection/macro.h"
#include "platform/string.h"

STRUCT()
struct MaterialResource
{
    PROPERTY()
    String BaseColorTexture;
    PROPERTY()
    String NormalTexture;
};

