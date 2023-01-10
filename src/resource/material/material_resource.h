#pragma

#include "core/reflection.h"
#include "platform/string.h"

STRUCT()
struct MaterialResource
{
    PROPERTY()
    String BaseColorTexture;
    PROPERTY()
    String NormalTexture;
};

