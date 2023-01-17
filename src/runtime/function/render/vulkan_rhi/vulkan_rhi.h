#pragma once

#include "function/render/rhi.h"
#include "function/render/vulkan_rhi/vulkan_rhi_resources.h"

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include <map>

class GLFWwindow;

class VulkanRHI final : public RHI
{
public:
    virtual void Initialize(RHIInitInfo init_info) override;

    virtual void CreateSwapChain() override;
    virtual void RecreateSwapChain() override;
    virtual void CreateSwapChainImageViews() override;
    virtual bool CreateFramebuffer(const vk::FramebufferCreateInfo* pCreateInfo, vk::Framebuffer& pFramebuffer);
    virtual void CreateImage(uint32_t                width,
                             uint32_t                height,
                             vk::Format              format,
                             vk::ImageTiling         tiling,
                             vk::ImageUsageFlags     usage,
                             vk::MemoryPropertyFlags properties,
                             vk::Image&              image,
                             vk::DeviceMemory&       image_memory,
                             vk::ImageCreateFlags    create_flags = {},
                             uint32_t                array_layers = 1,
                             uint32_t                mip_levels   = 1,
                             vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1);
    virtual void CreateImageView(vk::Image           image,
                                 vk::Format           format,
                                 vk::ImageAspectFlags image_aspect_flags,
                                 vk::ImageViewType    view_type,
                                 uint32_t             layout_count,
                                 uint32_t             miplevels,
                                 vk::ImageView&       image_view);

    virtual vk::CommandBuffer BeginSingleTimeCommands();
    virtual void              EndSingleTimeCommands(vk::CommandBuffer command_buffer);

    virtual void Clear() override;
    virtual void ClearSwapChain() override;

public:
    GLFWwindow* Window {nullptr};

    vk::Instance       Instance;
    vk::PhysicalDevice PhysicalDevice;
    vk::Device         Device;
    vk::SurfaceKHR     Surface;

    vk::DispatchLoaderDynamic DispatchDynamic;

    VmaAllocator Allocator;

    vk::DebugUtilsMessengerEXT DebugMessenger;

    vk::Queue GraphicsQueue;
    vk::Queue ComputeQueue;
    vk::Queue PresentQueue;

    vk::SwapchainKHR             SwapChain;
    vk::Format                   SwapChainImageFormat;
    std::vector<vk::Image>       SwapChainImages;
    std::vector<vk::ImageView>   SwapChainImageViews;
    std::vector<vk::Framebuffer> SwapChainFrameBuffers;

    vk::Image        ColorImage;
    vk::DeviceMemory ColorImageMemory;
    vk::ImageView    ColorImageView;

    vk::Extent2D SwapChainExtent;
    vk::Viewport Viewport;
    vk::Rect2D   Scissor;

    vk::Format       DepthImageFormat;
    vk::Image        DepthImage;
    vk::ImageView    DepthImageView;
    vk::DeviceMemory DepthImageMemory;

    vk::DescriptorPool DescriptorPool;

    vk::CommandPool                CommandPool;
    std::vector<vk::CommandBuffer> CcommandBuffers;

    std::vector<vk::Semaphore> ImageAvailableSemaphores;
    std::vector<vk::Semaphore> RenderFinishedSemaphores;
    std::vector<vk::Fence>     InFlightFences;

    uint32_t current_frame {0};
    bool     frame_buffer_resized {false};

    QueueFamilyIndices QueueIndices;

    vk::Sampler                     LinearSampler;
    vk::Sampler                     NearestSampler;
    std::map<uint32_t, vk::Sampler> m_mipmap_sampler_map;

    vk::SampleCountFlagBits MsaaSamples {vk::SampleCountFlagBits::e1};

    
    std::vector<const char*> ValidationLayers {"VK_LAYER_KHRONOS_validation"};
    bool                     EnableValidationLayers {false};
    bool                     EnableDebugUtilsLabel {false};

    std::vector<char const*> DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};


private:
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateWindowSurface();
    void SelectPhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandPool();
    void CreateCommandBuffer();
    void CreateDescriptorPool();
    void CreateSyncObjects();
    void CreateAllocator();

    void CreateColorResources();
    void CreateDepthResources();

    bool                     CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    bool                     IsDeviceSuitable(vk::PhysicalDevice device);
    vk::SampleCountFlagBits  GetMaxUsableSampleCount();
    QueueFamilyIndices       FindQueueFamilies(vk::PhysicalDevice physical_device);
    bool                     IsDeviceExtensionSupport(vk::PhysicalDevice device);
    SwapChainSupportDetails  QuerySwapChainSupport(vk::PhysicalDevice device);
    vk::Format               FindDepthFormat();

    vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates,
                                   vk::ImageTiling                tiling,
                                   vk::FormatFeatureFlags         features);

    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats);
    vk::PresentModeKHR   ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes);
    vk::Extent2D         ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
};
