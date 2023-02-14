#pragma once

#include <unordered_map>

constexpr size_t InvalidGuid = 0;

template<typename T>
class GuidAllocator
{
public:
    static bool IsValidGuid(size_t guid) {
        return guid != InvalidGuid;
    }

    size_t AllocGuid(const T& t)
    {
        if (auto iter = ElementsGuidMap.find(t); iter != ElementsGuidMap.end())
        {
            return iter->second;
        }

        for (size_t i = 0; i < GuidElementsMap.size() + 1; i++)
        {
            size_t guid = i + 1;
            if (GuidElementsMap.find(guid) == GuidElementsMap.end())
            {
                GuidElementsMap.insert(std::make_pair(guid, t));
                ElementsGuidMap.insert(std::make_pair(t, guid));
                return guid;
            }
        }

        return InvalidGuid;
    }

    bool GetElementGuid(const T& t, size_t& guid)
    {
        if (auto iter = ElementsGuidMap.find(t); iter != ElementsGuidMap.end())
        {
            guid = iter->second;
            return true;
        }

        return false;
    }

    bool HasElement(const T& t) {
        return ElementsGuidMap.find(t) != ElementsGuidMap.end();
    }

    void FreeGuid(size_t guid)
    {
        if (auto iter = GuidElementsMap.find(guid); iter != GuidElementsMap.end())
        {
            GuidElementsMap.erase(iter);
            ElementsGuidMap.erase(iter->second);
        }
    }

    void FreeElement(const T& t) {
        if (auto iter = ElementsGuidMap.find(t); iter != ElementsGuidMap.end())
        {
            ElementsGuidMap.erase(iter);
            GuidElementsMap.erase(iter->second);
        }
    }

    void Clear()
    {
        ElementsGuidMap.clear();
        GuidElementsMap.clear();
    }

private:
    std::unordered_map<T, size_t> ElementsGuidMap;
    std::unordered_map<size_t, T> GuidElementsMap;
};
