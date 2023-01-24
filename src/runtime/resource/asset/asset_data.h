#pragma once

#include "platform/string.h"

#include <memory>

class AssetData
{
public:
    AssetData() {}

    template<typename T>
    AssetData(const String& url, const std::shared_ptr<T>& ptr) : Url(url) {
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

    String               Url;
    std::weak_ptr<void*> Data;
};
