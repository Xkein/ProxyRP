#pragma once

#include <assert.h>

#define ENGINE_NAME "ProxyRendering"

#ifdef NDEBUG
#define ASSERT(statement)
#else
#define ASSERT(statement) assert(statement)
#endif // NDEBUG
