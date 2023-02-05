#pragma once

#include "function/render/rhi.h"
#include "function/render/vulkan_rhi/vulkan_rhi_resources.h"

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include <map>

struct GLFWwindow;

class VulkanRHI final : public RHI
{
public:
    virtual ~VulkanRHI() override;

    virtual void Initialize(RHIInitInfo init_info) override;
    virtual void PrepareContext() override;

    // allocate and create
    virtual RHICommandBuffer* AllocateCommandBuffers(const RHICommandBufferAllocateInfo* pAllocateInfo) override;
    virtual RHIDescriptorSet* AllocateDescriptorSets(const RHIDescriptorSetAllocateInfo* allocate_info) override;

    virtual void CreateSwapChain() override;
    virtual void RecreateSwapChain() override;
    virtual void CreateSwapChainImageViews() override;
    virtual RHIFramebuffer* CreateFramebuffer(const RHIFramebufferCreateInfo* pCreateInfo) override;
    virtual RHIRenderPass* CreateRenderPass(const RHIRenderPassCreateInfo* create_info) override;
    virtual RHIDescriptorSetLayout* CreateDescriptorSetLayout(const RHIDescriptorSetLayoutCreateInfo* create_info) override;
    virtual RHIPipelineLayout*      CreatePipelineLayout(const RHIPipelineLayoutCreateInfo* create_info) override;
    virtual RHIPipeline*            CreateGraphicsPipeline(RHIPipelineCache*                    pipeline_cache,
                                                           const RHIGraphicsPipelineCreateInfo* create_info) override;
    virtual RHIDescriptorPool*      CreateDescriptorPool(const RHIDescriptorPoolCreateInfo* create_info) override;

    virtual RHISampler* GetOrCreateDefaultSampler(RHIDefaultSamplerType type) override;
    virtual RHISampler* GetOrCreateMipmapSampler(uint32_t width, uint32_t height) override;
    virtual RHIShader* CreateShaderModule(const std::vector<byte>& shader_code) override;

    virtual void CreateBuffer(RHIDeviceSize          size,
                              RHIBufferUsageFlags    usage,
                              RHIMemoryPropertyFlags properties,
                              RHIBuffer*&             buffer,
                              RHIDeviceMemory*&       buffer_memory) override;
    virtual void CreateBufferAndInitialize(RHIDeviceSize          size,
                                           RHIBufferUsageFlags    usage,
                                           RHIMemoryPropertyFlags properties,
                                           RHIBuffer*&             buffer,
                                           RHIDeviceMemory*&       buffer_memory,
                                           void*                   data      = nullptr,
                                           int                     data_size = 0) override;
    virtual void CreateImage(uint32_t                width,
                             uint32_t                height,
                             RHIFormat              format,
                             RHIImageTiling         tiling,
                             RHIImageUsageFlags     usage,
                             RHIMemoryPropertyFlags properties,
                             RHIImage*&              image,
                             RHIDeviceMemory*&       image_memory,
                             RHIImageCreateFlags    create_flags = {},
                             uint32_t                array_layers = 1,
                             uint32_t                mip_levels   = 1,
                             RHISampleCountFlagBits sample_count = RHISampleCountFlagBits::e1) override;

    virtual void CreateImageView(const RHIImage*      image,
                                 RHIFormat           format,
                                 RHIImageAspectFlags image_aspect_flags,
                                 RHIImageViewType    view_type,
                                 uint32_t             layout_count,
                                 uint32_t             miplevels,
                                 RHIImageView*&       image_view) override;

    virtual void CreateTextureImage(RHIImage*&         image,
                                    RHIImageView*&     image_view,
                                    RHIDeviceMemory*&  image_memory,
                                    const TextureData* texure_data) override;

    virtual void CopyBuffer(RHIBuffer*     src_buffer,
                            RHIBuffer*     dst_buffer,
                            RHIDeviceSize src_offset,
                            RHIDeviceSize dst_offset,
                            RHIDeviceSize size) override;


    // command
    virtual bool              PrepareBeforePass(std::function<void()> on_recreate_swapchain) override;
    virtual void              SubmitRendering(std::function<void()> on_recreate_swapchain) override;
    virtual RHICommandBuffer* BeginSingleTimeCommands() override;
    virtual void              EndSingleTimeCommands(RHICommandBuffer* command_buffer) override;
    virtual bool              BeginCommandBuffer(RHICommandBuffer* command_buffer, const RHICommandBufferBeginInfo* begin_info) override;
    virtual bool              EndCommandBuffer(RHICommandBuffer* command_buffer) override;
    virtual void              BlitImage(RHICommandBuffer*             command_buffer,
                                        RHIImage*                     src_image,
                                        RHIImageLayout                src_image_layout,
                                        RHIImage*                     dst_image,
                                        RHIImageLayout                dst_image_layout,
                                        std::span<const RHIImageBlit> regions,
                                        RHIFilter                     filter) override;
    
    virtual void BeginRenderPass(RHICommandBuffer* command_buffer, const RHIRenderPassBeginInfo* begin_info, RHISubpassContents contents) override;
    virtual void NextSubpass(RHICommandBuffer* command_buffer, RHISubpassContents contents) override;
    virtual void EndRenderPass(RHICommandBuffer* command_buffer) override;
    virtual void BindPipeline(RHICommandBuffer* command_buffer, RHIPipelineBindPoint pipelineBindPoint, RHIPipeline* pipeline) override;
    virtual void BindDescriptorSets(RHICommandBuffer*                  command_buffer,
                                    RHIPipelineBindPoint               pipeline_bind_point,
                                    RHIPipelineLayout*                 layout,
                                    uint32_t                           first_set,
                                    std::span<const RHIDescriptorSet*> descriptor_sets,
                                    std::span<const uint32_t>          dynamic_offsets) override;
    virtual void BindVertexBuffers(RHICommandBuffer* command_buffer, uint32_t first_binding, std::span<const RHIBuffer*> buffers, std::span<const RHIDeviceSize> offsets) override;
    virtual void BindIndexBuffer(RHICommandBuffer* command_buffer, RHIBuffer* buffer, RHIDeviceSize offset, RHIIndexType indexType) override;
    virtual void UpdateDescriptorSets(std::span<const RHIWriteDescriptorSet> escriptor_writes, std::span<const RHICopyDescriptorSet> escriptor_copies) override;
    virtual void DrawIndexed(RHICommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;

    virtual void PushEvent(RHICommandBuffer* command_buffer, const Char* name, std::array<float, 4> color) override;
    virtual void PopEvent(RHICommandBuffer* command_buffer) override;
    virtual void SetViewport(RHICommandBuffer* command_buffer, uint32_t first_viewpor, std::span<const RHIViewport> viewports) override;
    virtual void SetScissor(RHICommandBuffer* command_buffer, uint32_t first_scissor, std::span<const RHIRect2D> scissors) override;

    virtual void ResetCommandPool() override;
    virtual void WaitForFences() override;

    // query
    virtual RHIPhysicalDeviceProperties GetPhysicalDeviceProperties() override;
    virtual RHICommandBuffer*  GetCommandBuffer() const override;
    virtual RHICommandPool*  GetCommandPool() const override;
    virtual RHIDescriptorPool* GetDescriptorPool() const override;
    virtual RHISwapchainDesc   GetSwapchainInfo() override;
    virtual RHIDepthImageDesc  GetDepthImageInfo() const override;
    virtual uint8_t            GetMaxFramesInFlight() const override;
    virtual uint8_t            GetCurrentFrameIndex() const override;
    virtual uint8_t            GetCurrentSwapchainIndex() const override;
    virtual RHISampleCountFlagBits GetMsaaSampleCount() const override;
    
    // destroy
    virtual void Clear() override;
    virtual void ClearSwapChain() override;
    virtual void DestroyBuffer(RHIBuffer* buffer) override;
    virtual void DestroyImage(RHIImage* image) override;
    virtual void DestroyImageView(RHIImageView* image_view) override;
    virtual void DestroyShaderModule(RHIShader* shader) override;
    virtual void DestroyFramebuffer(RHIFramebuffer* framebuffer) override;

    // memory
    virtual void FreeMemory(RHIDeviceMemory* memory) override;
    virtual void* MapMemory(RHIDeviceMemory* memory, vk::DeviceSize offset, vk::DeviceSize size, vk::MemoryMapFlags flags) override;
    virtual void UnmapMemory(RHIDeviceMemory* memory) override;

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

    vk::SwapchainKHR             SwapChain;
    vk::Format                   SwapChainImageFormat;
    std::vector<RHIImage*>       SwapChainImages;
    std::vector<RHIImageView*>   SwapChainImageViews;
    std::vector<vk::Framebuffer> SwapChainFrameBuffers;

    uint32_t CurrentSwapchainImageIndex;

    RHIImage*        ColorImage;
    RHIDeviceMemory* ColorImageMemory;
    RHIImageView*    ColorImageView;

    vk::Extent2D SwapChainExtent;
    vk::Viewport Viewport;
    vk::Rect2D   Scissor;

    vk::Format         DepthImageFormat;
    RHIImage*          DepthImage;
    RHIImageView*      DepthImageView;
    RHIDeviceMemory*   DepthImageMemory;

    VulkanDescriptorPool* DescriptorPool;

    VulkanCommandPool*               DefaultCommandPool;
    std::vector<RHICommandPoolRef>   CommandPools;
    std::vector<RHICommandBufferRef> CommandBuffers;

    VulkanCommandBuffer*            CurrentCommandBuffer;

    std::vector<vk::Semaphore> ImageAvailableSemaphores;
    std::vector<vk::Semaphore> RenderFinishedSemaphores;
    std::vector<vk::Fence>     InFlightFences;

    uint32_t CurrentFrameIndex {0};
    bool     FrameBufferResized {false};

    QueueFamilyIndices QueueIndices;

    VulkanSampler*                     LinearSampler;
    VulkanSampler*                     NearestSampler;
    std::map<uint32_t, RHISampler*>    MipmapSamplerMap;

    vk::SampleCountFlagBits MsaaSamples {vk::SampleCountFlagBits::e1};

    
    std::vector<const char*> ValidationLayers {"VK_LAYER_KHRONOS_validation"};
    bool                     EnableValidationLayers {false};
    bool                     EnableDebugUtilsLabel {false};

    uint32_t VulkanApiVersion {VK_API_VERSION_1_3};

    std::vector<char const*> DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME};

    uint32_t MaxVertexBlendingMeshCount {256};
    uint32_t MaxMaterialCount {256};

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
