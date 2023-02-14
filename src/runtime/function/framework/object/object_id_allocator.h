#pragma once

#include <atomic>
#include <limits>

using GameObjectID = size_t;

constexpr GameObjectID InvalidGObjectID = std::numeric_limits<std::size_t>::max();

class ObjectIDAllocator
{
public:
    static GameObjectID Alloc();

private:
    static std::atomic<GameObjectID> NextID;
};
