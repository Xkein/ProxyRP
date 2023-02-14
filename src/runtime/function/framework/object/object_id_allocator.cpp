#include "object_id_allocator.h"

#include "core/log/log_system.h"

std::atomic<GameObjectID> ObjectIDAllocator::NextID {0};

GameObjectID ObjectIDAllocator::Alloc()
{
    GameObjectID net_object_id = NextID.load();
    NextID++;
    if (NextID >= InvalidGObjectID)
    {
        LOG_FATAL("game object id overflow");
    }

    return net_object_id;
}
