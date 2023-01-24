#pragma once

#include "function/render/rhi.h"
#include "function/render/vulkan_rhi/vulkan_rhi_resources.h"
#include "platform/platform.h"

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include <map>

class GLFWwindow;
class TextureData;

class VulkanRHI final : public RHI
{
public:
    virtual ~VulkanRHI() override;

    virtual void Initialize(RHIInitInfo init_info) override;
    virtual void PrepareContext() override;

    // allocate and create
    virtual void CreateSwapChain() override;
    virtual void RecreateSwapChain() override;
    virtual void CreateSwapChainImageViews() override;
    virtual bool CreateFramebuffer(const vk::FramebufferCreateInfo* pCreateInfo, vk::Framebuffer& pFramebuffer);

    virtual RHIShader* CreateShaderModule(const std::vector<byte>& shader_code);

    virtual void CreateBuffer(vk::DeviceSize          size,
                              vk::BufferUsageFlags    usage,
                              vk::MemoryPropertyFlags properties,
                              vk::Buffer&             buffer,
                              vk::DeviceMemory&       buffer_memory);
    virtual void CreateBufferAndInitialize(vk::DeviceSize          size,
                                           vk::BufferUsageFlags    usage,
                                           vk::MemoryPropertyFlags properties,
                                           vk::Buffer&             buffer,
                                           vk::DeviceMemory&       buffer_memory,
                                           void*                   data      = nullptr,
                                           int                     data_size = 0);

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

    virtual void CreateTextureImage(vk::Image&         image,
                                    vk::ImageView&     image_view,
                                    vk::DeviceMemory&  device_memory,
                                    const TextureData* texure_data);

    virtual void CopyBuffer(vk::Buffer     srcBuffer,
                            vk::Buffer     dstBuffer,
                            vk::DeviceSize srcOffset,
                            vk::DeviceSize dstOffset,
                            vk::DeviceSize size);

    // command
    virtual vk::CommandBuffer BeginSingleTimeCommands();
    virtual void              EndSingleTimeCommands(vk::CommandBuffer command_buffer);

    // query
    virtual vk::CommandBuffer  GetCommandBuffer() const;
    virtual vk::DescriptorPool GetDescriptorPool() const;
    
    // destroy
    virtual void Clear() override;
    virtual void ClearSwapChain() override;

public:
    GLFWwindow* Window {nullptr};

    VulkanInstance       Instance;
    VulkanPhysicalDevice PhysicalDevice;
    VulkanDevice         Device;
    vk::SurfaceKHR       Surface;

    vk::DispatchLoaderDynamic DispatchDynamic;

    VmaAllocator Allocator;

    vk::DebugUtilsMessengerEXT DebugMessenger;

    VulkanQueue GraphicsQueue;
    VulkanQueue ComputeQueue;
    VulkanQueue PresentQueue;

    vk::SwapchainKHR               SwapChain;
    vk::Format                     SwapChainImageFormat;
    std::vector<vk::Image>       SwapChainImages;
    std::vector<vk::ImageView>   SwapChainImageViews;
    std::vector<vk::Framebuffer> SwapChainFrameBuffers;

    VulkanImage        ColorImage;
    VulkanDeviceMemory ColorImageMemory;
    VulkanImageView    ColorImageView;

    vk::Extent2D SwapChainExtent;
    vk::Viewport Viewport;
    vk::Rect2D   Scissor;

    vk::Format         DepthImageFormat;
    VulkanImage        DepthImage;
    VulkanImageView    DepthImageView;
    VulkanDeviceMemory DepthImageMemory;

    VulkanDescriptorPool DescriptorPool;

    VulkanCommandPool              CommandPool;
    std::vector<vk::CommandBuffer> CommandBuffers;

    VulkanCommandBuffer            CurrentCommandBuffer;

    std::vector<vk::Semaphore> ImageAvailableSemaphores;
    std::vector<vk::Semaphore> RenderFinishedSemaphores;
    std::vector<vk::Fence>     InFlightFences;

    uint32_t CurrentFrame {0};
    bool     FrameBufferResized {false};

    QueueFamilyIndices QueueIndices;

    VulkanSampler                     LinearSampler;
    VulkanSampler                     NearestSampler;
    std::map<uint32_t, VulkanSampler> MipmapSamplerMap;

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
    QueueFamilyIndices       FindQueueFamilies(vk::PhysicalDevice physical_device);
    bool                     IsDeviceExtensionSupport(vk::PhysicalDevice device);
    SwapChainSupportDetails  QuerySwapChainSupport(vk::PhysicalDevice device);


    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats);
    vk::PresentModeKHR   ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes);
    vk::Extent2D         ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
};
