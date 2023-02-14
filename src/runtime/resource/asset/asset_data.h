#pragma once

#include "platform/string.h"

#include <memory>

class AssetData
{
public:
    AssetData() {}

    template<typename T>
    AssetData(size_t hash, const std::shared_ptr<T>& ptr) : Hash(hash) {
        Data = std::reinterpret_pointer_cast<void*>(ptr);
    }

    template<typename T>
    std::shared_ptr<T> GetData() {
        if (Data.expired())
        {
            return std::shared_ptr<T>();
        }

        return std::reinterpret_pointer_cast<T>(Data.lock());
    }

    size_t               Hash {0};
    std::weak_ptr<void*> Data;
};
