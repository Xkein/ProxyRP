#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices
{
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;
    std::optional<uint32_t> ComputeFamily;

    bool isComplete()
    {
        return GraphicsFamily.has_value() && PresentFamily.has_value() && ComputeFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR        Capabilities;
    std::vector<vk::SurfaceFormatKHR> Formats;
    std::vector<vk::PresentModeKHR>   PresentModes;
};
