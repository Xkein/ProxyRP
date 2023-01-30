#include "vulkan_rhi.h"
#include "vulkan_util.h"
#include "vulkan_rhi_converter.h"
#include "core/macro.h"
#include "core/log/log_system.h"
#include "function/ui/window_system.h"

#include <GLFW/glfw3.h>
#include <set>

VulkanRHI::~VulkanRHI() {}

void VulkanRHI::Initialize(RHIInitInfo init_info)
{
    Window = init_info.WindowSystem->GetWindow();

    auto window_size = init_info.WindowSystem->GetWindowSize();

    Viewport = vk::Viewport{0.0f, 0.0f, (float)window_size[0], (float)window_size[1], 0.0f, 1.0f};
    Scissor  = vk::Rect2D {{0, 0}, {(uint32_t)window_size[0], (uint32_t)window_size[1]}};

#ifndef NDEBUG
    EnableValidationLayers = true;
    EnableDebugUtilsLabel  = true;
#else
    EnableValidationLayers = false;
    EnableDebugUtilsLabel  = false;
#endif

    CreateInstance();

    SetupDebugMessenger();

    CreateWindowSurface();

    SelectPhysicalDevice();
    CreateLogicalDevice();

    CreateCommandPool();
    CreateCommandBuffer();

    CreateDescriptorPool();

    CreateSyncObjects();

    CreateSwapChain();
    CreateSwapChainImageViews();

    CreateColorResources();
    CreateDepthResources();

    CreateAllocator();
}

void VulkanRHI::PrepareContext()
{
    CurrentCommandBuffer = (VulkanCommandBuffer*)CommandBuffers[CurrentFrame].get();
}

RHIDescriptorSet* VulkanRHI::AllocateDescriptorSets(const RHIDescriptorSetAllocateInfo* allocate_info)
{
    vk::DescriptorSetAllocateInfo vk_allocate_info = VulkanRHIConverter::Convert(*allocate_info);
    vk::DescriptorSet             descriptor_set   = Device->allocateDescriptorSets(vk_allocate_info)[0];

    return new VulkanDescriptorSet(descriptor_set);
}

void VulkanRHI::CreateSwapChain()
{
    SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(PhysicalDevice);

    vk::SurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.Formats);
    vk::PresentModeKHR   present_mode   = ChooseSwapPresentMode(swap_chain_support.PresentModes);
    vk::Extent2D         extent         = ChooseSwapExtent(swap_chain_support.Capabilities);

    uint32_t image_count = swap_chain_support.Capabilities.minImageCount + 1;
    if (swap_chain_support.Capabilities.maxImageCount > 0 &&
        image_count > swap_chain_support.Capabilities.maxImageCount)
    {
        image_count = swap_chain_support.Capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR create_info {.surface          = Surface,
                                            .minImageCount    = image_count,
                                            .imageFormat      = surface_format.format,
                                            .imageColorSpace  = surface_format.colorSpace,
                                            .imageExtent      = extent,
                                            .imageArrayLayers = 1,
                                            .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment};

    QueueFamilyIndices indices              = FindQueueFamilies(PhysicalDevice);
    uint32_t           queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};

    if (indices.GraphicsFamily != indices.PresentFamily)
    {
        create_info.imageSharingMode      = vk::SharingMode::eConcurrent;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices   = queueFamilyIndices;
    }
    else
    {
        create_info.imageSharingMode      = vk::SharingMode::eExclusive;
        create_info.queueFamilyIndexCount = 0;       // Optional
        create_info.pQueueFamilyIndices   = nullptr; // Optional
    }

    create_info.preTransform   = swap_chain_support.Capabilities.currentTransform;
    create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    create_info.presentMode    = present_mode;
    create_info.clipped        = VK_TRUE;
    create_info.oldSwapchain   = VK_NULL_HANDLE;

    SwapChain = Device->createSwapchainKHR(create_info);

    std::vector<vk::Image> swapchain_images  = Device->getSwapchainImagesKHR(SwapChain);
    SwapChainImages.resize(swapchain_images.size());
    std::transform(swapchain_images.begin(), swapchain_images.end(), SwapChainImages.begin(), [](vk::Image image) {
        return new VulkanImage(image);
    });
    SwapChainImageFormat = surface_format.format;
    SwapChainExtent       = extent;

    Scissor = vk::Rect2D{{0, 0}, {SwapChainExtent.width, SwapChainExtent.height}};
}

void VulkanRHI::RecreateSwapChain()
{
    int width = 0, height = 0;
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(Window, &width, &height);
        glfwWaitEvents();
    }
    
    vk::Result result_wait = Device->waitForFences(InFlightFences, VK_TRUE, UINT64_MAX);
    if (result_wait != vk::Result::eSuccess)
    {
        LOG_ERROR("waitForFences failed");
        return;
    }

    ClearSwapChain();

    CreateSwapChain();
    CreateSwapChainImageViews();
    CreateColorResources();
    CreateDepthResources();
}

void VulkanRHI::CreateSwapChainImageViews()
{
    SwapChainImageViews.resize(SwapChainImages.size());

    for (size_t i = 0; i < SwapChainImages.size(); i++)
    {
        CreateImageView(SwapChainImages[i],
                        SwapChainImageFormat,
                        vk::ImageAspectFlagBits::eColor,
                        vk::ImageViewType::e2D,
                        1,
                        1,
                        SwapChainImageViews[i]);
    }
}

RHIFramebuffer* VulkanRHI::CreateFramebuffer(const RHIFramebufferCreateInfo* pCreateInfo)
{
    vk::FramebufferCreateInfo vk_create_info = VulkanRHIConverter::Convert(*pCreateInfo);

    int                        image_view_size = vk_create_info.attachmentCount;
    std::vector<vk::ImageView> vk_image_view_list(image_view_size);
    for (int i = 0; i < image_view_size; ++i)
    {
        const auto& rhi_image_view_element = vk_create_info.pAttachments[i];
        auto&       vk_image_view_element  = vk_image_view_list[i];

        vk_image_view_element = rhi_image_view_element;
    };

    vk::FramebufferCreateInfo create_info {};
    create_info.pNext           = vk_create_info.pNext;
    create_info.flags           = vk_create_info.flags;
    create_info.renderPass      = vk_create_info.renderPass;
    create_info.attachmentCount = vk_create_info.attachmentCount;
    create_info.pAttachments    = vk_image_view_list.data();
    create_info.width           = vk_create_info.width;
    create_info.height          = vk_create_info.height;
    create_info.layers          = vk_create_info.layers;

    vk::Framebuffer framebuffer = Device->createFramebuffer(create_info);

    return new VulkanFramebuffer(framebuffer);
}

RHIRenderPass* VulkanRHI::CreateRenderPass(const RHIRenderPassCreateInfo* create_info)
{
    vk::RenderPassCreateInfo vk_create_info = VulkanRHIConverter::Convert(*create_info);
    vk::RenderPass render_pass = Device->createRenderPass(vk_create_info);

    return new VulkanRenderPass(render_pass);
}

RHIDescriptorSetLayout* VulkanRHI::CreateDescriptorSetLayout(const RHIDescriptorSetLayoutCreateInfo* create_info)
{
    vk::DescriptorSetLayoutCreateInfo vk_create_info        = VulkanRHIConverter::Convert(*create_info);
    vk::DescriptorSetLayout           descriptor_set_layout = Device->createDescriptorSetLayout(vk_create_info);

    return new VulkanDescriptorSetLayout(descriptor_set_layout);
}

RHIPipelineLayout* VulkanRHI::CreatePipelineLayout(const RHIPipelineLayoutCreateInfo* create_info)
{
    vk::PipelineLayoutCreateInfo vk_create_info  = VulkanRHIConverter::Convert(*create_info);
    vk::PipelineLayout           pipeline_layout = Device->createPipelineLayout(vk_create_info);

    return new VulkanPipelineLayout(pipeline_layout);
}

RHISampler* VulkanRHI::GetOrCreateMipmapSampler(uint32_t width, uint32_t height)
{
    uint32_t mip_levels = floor(log2(std::max(width, height))) + 1;
    if (auto iter = MipmapSamplerMap.find(mip_levels); iter != MipmapSamplerMap.end())
    {
        return iter->second;
    }

    vk::PhysicalDeviceProperties properties = PhysicalDevice->getProperties();

    vk::SamplerCreateInfo sampler_info {
        .magFilter               = vk::Filter::eLinear,
        .minFilter               = vk::Filter::eLinear,
        .mipmapMode              = vk::SamplerMipmapMode::eLinear,
        .addressModeU            = vk::SamplerAddressMode::eRepeat,
        .addressModeV            = vk::SamplerAddressMode::eRepeat,
        .addressModeW            = vk::SamplerAddressMode::eRepeat,
        .mipLodBias              = 0.0f,
        .anisotropyEnable        = VK_TRUE,
        .maxAnisotropy           = properties.limits.maxSamplerAnisotropy,
        .compareEnable           = VK_FALSE,
        .compareOp               = vk::CompareOp::eAlways,
        .minLod                  = 0.0f,
        .maxLod                  = (float)mip_levels,
        .borderColor             = vk::BorderColor::eIntOpaqueBlack,
        .unnormalizedCoordinates = VK_FALSE,
    };

    RHISampler* sampler = new VulkanSampler(Device->createSampler(sampler_info));
    MipmapSamplerMap[mip_levels] = sampler;

    return sampler;
}

RHISampler* VulkanRHI::GetOrCreateDefaultSampler(RHIDefaultSamplerType type)
{
    switch (type)
    {
        case RHIDefaultSamplerType::Linear:
            if (LinearSampler == nullptr)
            {
                vk::PhysicalDeviceProperties properties = PhysicalDevice->getProperties();

                vk::SamplerCreateInfo sampler_info {
                    .magFilter               = vk::Filter::eLinear,
                    .minFilter               = vk::Filter::eLinear,
                    .mipmapMode              = vk::SamplerMipmapMode::eLinear,
                    .addressModeU            = vk::SamplerAddressMode::eRepeat,
                    .addressModeV            = vk::SamplerAddressMode::eRepeat,
                    .addressModeW            = vk::SamplerAddressMode::eRepeat,
                    .mipLodBias              = 0.0f,
                    .anisotropyEnable        = VK_TRUE,
                    .maxAnisotropy           = properties.limits.maxSamplerAnisotropy,
                    .compareEnable           = VK_FALSE,
                    .compareOp               = vk::CompareOp::eAlways,
                    .minLod                  = 0.0f,
                    .maxLod                  = 8,
                    .borderColor             = vk::BorderColor::eIntOpaqueBlack,
                    .unnormalizedCoordinates = VK_FALSE,
                };

                *LinearSampler = Device->createSampler(sampler_info);
            }
            return LinearSampler;
        case RHIDefaultSamplerType::Nearest:
            if (NearestSampler == nullptr)
            {
                vk::PhysicalDeviceProperties properties = PhysicalDevice->getProperties();

                vk::SamplerCreateInfo sampler_info {
                    .magFilter               = vk::Filter::eNearest,
                    .minFilter               = vk::Filter::eNearest,
                    .mipmapMode              = vk::SamplerMipmapMode::eNearest,
                    .addressModeU            = vk::SamplerAddressMode::eRepeat,
                    .addressModeV            = vk::SamplerAddressMode::eRepeat,
                    .addressModeW            = vk::SamplerAddressMode::eRepeat,
                    .mipLodBias              = 0.0f,
                    .anisotropyEnable        = VK_TRUE,
                    .maxAnisotropy           = properties.limits.maxSamplerAnisotropy,
                    .compareEnable           = VK_FALSE,
                    .compareOp               = vk::CompareOp::eAlways,
                    .minLod                  = 0.0f,
                    .maxLod                  = 8,
                    .borderColor             = vk::BorderColor::eIntOpaqueBlack,
                    .unnormalizedCoordinates = VK_FALSE,
                };

                *NearestSampler = Device->createSampler(sampler_info);
            }
            return NearestSampler;
    }
    return nullptr;
}

RHIShader* VulkanRHI::CreateShaderModule(const std::vector<byte>& shader_code)
{
    vk::ShaderModule shader = VulkanUtil::CreateShaderModule(Device, shader_code);

    return new VulkanShader(shader);
}

RHIPipeline* VulkanRHI::CreateGraphicsPipeline(RHIPipelineCache*                    pipeline_cache,
                                               const RHIGraphicsPipelineCreateInfo* create_info)
{
    vk::GraphicsPipelineCreateInfo vk_create_info = VulkanRHIConverter::Convert(*create_info);
    vk::PipelineCache              vk_pipeline_cache =
        pipeline_cache == VK_NULL_HANDLE ? VK_NULL_HANDLE : *(VulkanPipelineCache*)pipeline_cache;
    vk::Pipeline pipeline = Device->createGraphicsPipeline(vk_pipeline_cache, vk_create_info).value;

    return new VulkanPipeline(pipeline);
}

RHIDescriptorPool* VulkanRHI::CreateDescriptorPool(const RHIDescriptorPoolCreateInfo* create_info) {
    vk::DescriptorPoolCreateInfo vk_create_info = VulkanRHIConverter::Convert(*create_info);
    vk::DescriptorPool           descriptor_pool = Device->createDescriptorPool(vk_create_info);

    return new VulkanDescriptorPool(descriptor_pool);
}

void VulkanRHI::CreateBuffer(vk::DeviceSize          size,
                             vk::BufferUsageFlags    usage,
                             vk::MemoryPropertyFlags properties,
                             RHIBuffer*&             buffer,
                             RHIDeviceMemory*&       buffer_memory)
{
    vk::Buffer       vulkan_buffer;
    vk::DeviceMemory vulkan_buffer_memory;
    VulkanUtil::CreateBuffer(PhysicalDevice, Device, size, usage, properties, vulkan_buffer, vulkan_buffer_memory);

    buffer        = new VulkanBuffer(vulkan_buffer);
    buffer_memory = new VulkanDeviceMemory(vulkan_buffer_memory);
}

void VulkanRHI::CreateBufferAndInitialize(vk::DeviceSize          size,
                                          vk::BufferUsageFlags    usage,
                                          vk::MemoryPropertyFlags properties,
                                          RHIBuffer*&             buffer,
                                          RHIDeviceMemory*&       buffer_memory,
                                          void*                   data,
                                          int                     data_size)
{
    vk::Buffer       vulkan_buffer;
    vk::DeviceMemory vulkan_buffer_memory;
    VulkanUtil::CreateBufferAndInitialize(
        Device, PhysicalDevice, usage, properties, vulkan_buffer, vulkan_buffer_memory, size, data, data_size);

    buffer        = new VulkanBuffer(vulkan_buffer);
    buffer_memory = new VulkanDeviceMemory(vulkan_buffer_memory);
}

void VulkanRHI::CreateImage(uint32_t                width,
                            uint32_t                height,
                            vk::Format              format,
                            vk::ImageTiling         tiling,
                            vk::ImageUsageFlags     usage,
                            vk::MemoryPropertyFlags properties,
                            RHIImage*&              image,
                            RHIDeviceMemory*&       image_memory,
                            vk::ImageCreateFlags    create_flags,
                            uint32_t                array_layers,
                            uint32_t                mip_levels,
                            vk::SampleCountFlagBits sample_count)
{
    vk::Image        vulkan_image;
    vk::DeviceMemory vulkan_image_memory;
    VulkanUtil::CreateImage(PhysicalDevice,
                            Device,
                            width,
                            height,
                            format,
                            tiling,
                            usage,
                            properties,
                            vulkan_image,
                            vulkan_image_memory,
                            create_flags,
                            array_layers,
                            mip_levels,
                            sample_count);

    image        = new VulkanImage(vulkan_image);
    image_memory = new VulkanDeviceMemory(vulkan_image_memory);
}

void VulkanRHI::CreateImageView(const RHIImage*      image,
                                vk::Format           format,
                                vk::ImageAspectFlags image_aspect_flags,
                                vk::ImageViewType    view_type,
                                uint32_t             layout_count,
                                uint32_t             miplevels,
                                RHIImageView*&       image_view)
{
    vk::ImageView    vulkan_image_view;
    vulkan_image_view = VulkanUtil::CreateImageView(
        Device, *(VulkanImage*)image, format, image_aspect_flags, view_type, layout_count, miplevels);

}

void VulkanRHI::CreateTextureImage(RHIImage*&         image,
                                   RHIImageView*&     image_view,
                                   RHIDeviceMemory*&  image_memory,
                                   const TextureData* texure_data)
{
    vk::Image        vulkan_image;
    vk::ImageView    vulkan_image_view;
    vk::DeviceMemory vulkan_image_memory;
    VulkanUtil::CreateTextureImage(this, vulkan_image, vulkan_image_view, vulkan_image_memory, texure_data);

    
    image        = new VulkanImage(vulkan_image);
    image_view   = new VulkanImageView(vulkan_image_view);
    image_memory = new VulkanDeviceMemory(vulkan_image_memory);
}

void VulkanRHI::CopyBuffer(RHIBuffer*     src_buffer,
                           RHIBuffer*     dst_buffer,
                           vk::DeviceSize src_offset,
                           vk::DeviceSize dst_offset,
                           vk::DeviceSize size)
{
    VulkanUtil::CopyBuffer(this, *(VulkanBuffer*)src_buffer, *(VulkanBuffer*)dst_buffer, src_offset, dst_offset, size);
}

RHICommandBuffer* VulkanRHI::BeginSingleTimeCommands()
{
    vk::CommandBufferAllocateInfo alloc_info {
        .commandPool        = CommandPool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };

    vk::CommandBuffer command_buffer = Device->allocateCommandBuffers(alloc_info)[0];

    vk::CommandBufferBeginInfo begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };

    command_buffer.begin(begin_info);

    return new VulkanCommandBuffer(command_buffer);
}

void VulkanRHI::EndSingleTimeCommands(RHICommandBuffer* command_buffer)
{
    vk::CommandBuffer vk_command_buffer = *(VulkanCommandBuffer*)command_buffer;
    vk_command_buffer.end();

    vk::SubmitInfo submit_info {
        .commandBufferCount = 1,
        .pCommandBuffers    = &vk_command_buffer,
    };

    GraphicsQueue->submit({submit_info});
    GraphicsQueue->waitIdle();

    Device->freeCommandBuffers(CommandPool, {vk_command_buffer});
    delete command_buffer;
}

void VulkanRHI::BeginRenderPass(RHICommandBuffer* command_buffer, const RHIRenderPassBeginInfo* begin_info, RHISubpassContents contents)
{
    vk::RenderPassBeginInfo vk_begin_info = VulkanRHIConverter::Convert(*begin_info);
    vk::CommandBuffer       vk_command_buffer = *(VulkanCommandBuffer*)command_buffer;

    vk_command_buffer.beginRenderPass(vk_begin_info, contents);
}

void VulkanRHI::NextSubpass(RHICommandBuffer* command_buffer, RHISubpassContents contents)
{
    vk::CommandBuffer vk_command_buffer = *(VulkanCommandBuffer*)command_buffer;

    vk_command_buffer.nextSubpass(contents);
}

void VulkanRHI::EndRenderPass(RHICommandBuffer* command_buffer)
{
    vk::CommandBuffer vk_command_buffer = *(VulkanCommandBuffer*)command_buffer;

    vk_command_buffer.endRenderPass();
}

void VulkanRHI::BindPipeline(RHICommandBuffer* command_buffer, RHIPipelineBindPoint pipelineBindPoint, RHIPipeline* pipeline)
{
    vk::CommandBuffer vk_command_buffer = *(VulkanCommandBuffer*)command_buffer;

    vk_command_buffer.bindPipeline(pipelineBindPoint, *(VulkanPipeline*)pipeline);
}
void VulkanRHI::BindDescriptorSets(RHICommandBuffer*                              command_buffer,
                                   RHIPipelineBindPoint                           pipeline_bind_point,
                                   RHIPipelineLayout*                             layout,
                                   uint32_t                                       first_set,
                                   vk::ArrayProxy<const RHIDescriptorSet*> const& descriptor_sets,
                                   vk::ArrayProxy<const uint32_t> const&          dynamic_offsets)
{
    vk::CommandBuffer vk_command_buffer = *(VulkanCommandBuffer*)command_buffer;
    
    std::vector<vk::DescriptorSet> vk_descriptor_sets(descriptor_sets.size());
    for (uint32_t i = 0; i < descriptor_sets.size(); i++)
    {
        vk_descriptor_sets[i] = (vk::DescriptorSet) * (VulkanDescriptorSet*)descriptor_sets.data()[i];
    }

    vk_command_buffer.bindDescriptorSets(pipeline_bind_point, *(VulkanPipelineLayout*)layout, first_set, vk_descriptor_sets, dynamic_offsets);
}

void VulkanRHI::BindVertexBuffers(RHICommandBuffer* command_buffer, uint32_t first_binding, vk::ArrayProxy<const RHIBuffer*> const& buffers, vk::ArrayProxy<const RHIDeviceSize> const& offsets)
{
    vk::CommandBuffer vk_command_buffer = *(VulkanCommandBuffer*)command_buffer;

    std::vector<vk::Buffer> vk_buffers(buffers.size());
    for (uint32_t i = 0; i < buffers.size(); i++)
    {
        vk_buffers[i] = (vk::Buffer) * (VulkanBuffer*)buffers.data()[i];
    }

    vk_command_buffer.bindVertexBuffers(first_binding, vk_buffers, offsets);
}

void VulkanRHI::BindIndexBuffer(RHICommandBuffer* command_buffer, RHIBuffer* buffer, RHIDeviceSize offset, RHIIndexType indexType)
{
    vk::CommandBuffer vk_command_buffer = *(VulkanCommandBuffer*)command_buffer;

    vk_command_buffer.bindIndexBuffer(*(VulkanBuffer*)buffer, offset, indexType);
}

void VulkanRHI::UpdateDescriptorSets(const vk::ArrayProxy<const RHIWriteDescriptorSet>& descriptor_writes,
                                     const vk::ArrayProxy<const RHICopyDescriptorSet>&  descriptor_copies)
{
    Device->updateDescriptorSets(descriptor_writes, descriptor_copies);
}

void VulkanRHI::PushEvent(RHICommandBuffer* commond_buffer, const Char* name, std::array<float, 4> color)
{
    if (!EnableDebugUtilsLabel)
        return;

    vk::DebugUtilsLabelEXT label_info {
        .pLabelName = name,
        .color      = color,
    };

    static_cast<VulkanCommandBuffer*>(commond_buffer)->Resource.beginDebugUtilsLabelEXT(label_info, DispatchDynamic);
}
void VulkanRHI::PopEvent(RHICommandBuffer* commond_buffer)
{
    static_cast<VulkanCommandBuffer*>(commond_buffer)->Resource.endDebugUtilsLabelEXT(DispatchDynamic);
}

RHIPhysicalDeviceProperties VulkanRHI::GetPhysicalDeviceProperties()
{
    return PhysicalDevice->getProperties();
}

RHICommandBuffer* VulkanRHI::GetCommandBuffer() const
{
    return CurrentCommandBuffer;
}

RHIDescriptorPool* VulkanRHI::GetDescriptorPool() const
{
    return DescriptorPool;
}

RHIDepthImageDesc VulkanRHI::GetDepthImageInfo() const
{
    return RHIDepthImageDesc {
        .DepthImage       = DepthImage,
        .DepthImageView   = DepthImageView,
        .DepthImageFormat = DepthImageFormat,
    };
}

RHISwapchainDesc VulkanRHI::GetSwapchainInfo()
{
    return RHISwapchainDesc {
        .ImageFormat = SwapChainImageFormat,
        .Extent      = SwapChainExtent,
        .Viewport    = &Viewport,
        .Scissor     = &Scissor,
        .ImageViews  = SwapChainImageViews,
    };
}

uint8_t VulkanRHI::GetMaxFramesInFlight() const
{
    return MAX_FRAMES_IN_FLIGHT;
}

uint8_t VulkanRHI::GetCurrentFrameIndex() const
{
    return CurrentFrame;
}

void VulkanRHI::Clear()
{
    if (EnableValidationLayers)
    {
        Instance->destroyDebugUtilsMessengerEXT(DebugMessenger, nullptr, DispatchDynamic);
    }
}

void VulkanRHI::ClearSwapChain()
{
    DestroyImageView(ColorImageView);
    DestroyImage(ColorImage);
    FreeMemory(ColorImageMemory);

    DestroyImageView(DepthImageView);
    DestroyImage(DepthImage);
    FreeMemory(DepthImageMemory);

    for (auto& image_view : SwapChainImageViews)
    {
        DestroyImageView(image_view);
    }
    Device->destroySwapchainKHR(SwapChain);
}

void VulkanRHI::DestroyBuffer(RHIBuffer* buffer)
{
    Device->destroyBuffer(*(VulkanBuffer*)buffer);
}

void VulkanRHI::DestroyImage(RHIImage* image)
{
    Device->destroyImage(*(VulkanImage*)image);
}

void VulkanRHI::DestroyImageView(RHIImageView* image_view)
{
    Device->destroyImageView(*(VulkanImageView*)image_view);
}

void VulkanRHI::DestroyShaderModule(RHIShader* shader)
{
    Device->destroyShaderModule(*(VulkanShader*)shader);
}

void VulkanRHI::FreeMemory(RHIDeviceMemory* memory)
{
    Device->freeMemory(*(VulkanDeviceMemory*)memory);
}

void* VulkanRHI::MapMemory(RHIDeviceMemory*   memory,
                          vk::DeviceSize     offset,
                          vk::DeviceSize     size,
                          vk::MemoryMapFlags flags)
{
    return Device->mapMemory(*(VulkanDeviceMemory*)memory, offset, size, flags);
}

void VulkanRHI::UnmapMemory(RHIDeviceMemory* memory)
{
    Device->unmapMemory(*(VulkanDeviceMemory*)memory);
}

void VulkanRHI::CreateInstance() {
    if (EnableValidationLayers && !CheckValidationLayerSupport())
    {
        LOG_ERROR("validation layers requestd, but not available!");
    }

    vk::ApplicationInfo appInfo {.pApplicationName   = "Proxy Rendering Process",
                                 .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                 .pEngineName        = ENGINE_NAME,
                                 .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
                                 .apiVersion         = VK_API_VERSION_1_3};

    std::vector<const char*> extensions = GetRequiredExtensions();

    vk::InstanceCreateInfo createInfo {.pApplicationInfo        = &appInfo,
                                       .enabledLayerCount       = 0,
                                       .enabledExtensionCount   = (uint32_t)extensions.size(),
                                       .ppEnabledExtensionNames = extensions.data()};

    if (EnableValidationLayers)
    {
        createInfo.enabledLayerCount   = ValidationLayers.size();
        createInfo.ppEnabledLayerNames = ValidationLayers.data();
    }

    Instance = vk::createInstance(createInfo);

    DispatchDynamic.init(vkGetInstanceProcAddr);
    DispatchDynamic.init(Instance);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT      message_serverity,
                                               vk::DebugUtilsMessageTypeFlagBitsEXT          message_type,
                                               const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
                                               void*                                         user_data)
{
    if (message_serverity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
        LOG_WARN("validation layer: {}", callback_data->pMessage);
    }
    else if (message_serverity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
    {
        LOG_ERROR("validation layer: {}", callback_data->pMessage);
    }
    return VK_FALSE;
}
void VulkanRHI::SetupDebugMessenger()
{
    if (!EnableValidationLayers)
        return;

    vk::DebugUtilsMessengerCreateInfoEXT create_info {
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                       vk::DebugUtilsMessageTypeFlagBitsEXT ::eValidation |
                       vk::DebugUtilsMessageTypeFlagBitsEXT ::ePerformance,
        .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)DebugCallback,
        .pUserData       = nullptr};

    DebugMessenger = Instance->createDebugUtilsMessengerEXT(create_info, nullptr, DispatchDynamic);
}

void VulkanRHI::CreateWindowSurface()
{
    if (glfwCreateWindowSurface(Instance, Window, nullptr, (VkSurfaceKHR*)&Surface) != VK_SUCCESS)
    {
        LOG_ERROR("glfwCreateWindowSurface failed !");
    }
}

void VulkanRHI::SelectPhysicalDevice()
{
    std::vector<vk::PhysicalDevice> physical_devices = Instance->enumeratePhysicalDevices();

    std::vector<std::pair<int, vk::PhysicalDevice>> ranked_physical_devices;
    for (const auto& device : physical_devices)
    {
        vk::PhysicalDeviceProperties physical_device_properties = device.getProperties();
        int score = 0;

        if (physical_device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {
            score += 1000;
        }
        else if (physical_device_properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
        {
            score += 100;
        }

        ranked_physical_devices.push_back({score, device});
    }

    std::sort(
        ranked_physical_devices.begin(),
        ranked_physical_devices.end(),
        [](const std::pair<int, vk::PhysicalDevice>& p1, const std::pair<int, vk::PhysicalDevice>& p2) { return p1 > p2; });

    for (const auto& [score, device] : ranked_physical_devices)
    {
        if (IsDeviceSuitable(device))
        {
            PhysicalDevice = device;
            MsaaSamples    = VulkanUtil::GetMaxUsableSampleCount(PhysicalDevice);
            break;
        }
    }

    if (!PhysicalDevice)
    {
        LOG_ERROR("failed to find GPUs with Vulkan support!");
        return;
    }
}

void VulkanRHI::CreateLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t>                     queue_families = {
        indices.GraphicsFamily.value(), indices.ComputeFamily.value(), indices.PresentFamily.value()};

    float queue_priority = 1.0f;
    for (uint32_t queue_family : queue_families)
    {
        vk::DeviceQueueCreateInfo info {
            .queueFamilyIndex = queue_family,
            .queueCount       = 1,
            .pQueuePriorities = &queue_priority,
        };
        queue_create_infos.push_back(info);
    }

    vk::PhysicalDeviceFeatures device_features {
        .independentBlend         = VK_TRUE,
        .geometryShader           = VK_TRUE,
        .samplerAnisotropy        = VK_TRUE,
        .fragmentStoresAndAtomics = VK_TRUE,
    };

    vk::DeviceCreateInfo create_info {.queueCreateInfoCount    = (uint32_t)queue_create_infos.size(),
                                      .pQueueCreateInfos       = queue_create_infos.data(),
                                      .enabledLayerCount       = 0,
                                      .ppEnabledLayerNames     = nullptr,
                                      .enabledExtensionCount   = (uint32_t)DeviceExtensions.size(),
                                      .ppEnabledExtensionNames = DeviceExtensions.data(),
                                      .pEnabledFeatures        = &device_features};

    Device = PhysicalDevice->createDevice(create_info);

    GraphicsQueue = Device->getQueue(indices.GraphicsFamily.value(), 0);
    ComputeQueue  = Device->getQueue(indices.ComputeFamily.value(), 0);
    PresentQueue  = Device->getQueue(indices.PresentFamily.value(), 0);

    DispatchDynamic.init(Device);

    DepthImageFormat = VulkanUtil::FindDepthFormat(PhysicalDevice);
}

void VulkanRHI::CreateCommandPool()
{
    QueueFamilyIndices queue_family_indices = FindQueueFamilies(PhysicalDevice);

    vk::CommandPoolCreateInfo pool_info {
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = queue_family_indices.GraphicsFamily.value(),
    };

    CommandPool = Device->createCommandPool(pool_info);
}

void VulkanRHI::CreateCommandBuffer()
{
    CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    vk::CommandBufferAllocateInfo allocate_info {.commandPool        = CommandPool,
                                                 .level              = vk::CommandBufferLevel::ePrimary,
                                                 .commandBufferCount = (uint32_t)CommandBuffers.size()};

    std::vector<vk::CommandBuffer> vk_command_buffers = Device->allocateCommandBuffers(allocate_info);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        CommandBuffers[i] = RHICommandBufferRef(new VulkanCommandBuffer(vk_command_buffers[i]));
    }
}

void VulkanRHI::CreateDescriptorPool()
{
    std::array<vk::DescriptorPoolSize, 7> pool_sizes;
    pool_sizes[0].type            = vk::DescriptorType::eStorageBufferDynamic;
    pool_sizes[0].descriptorCount = 15;
    pool_sizes[1].type            = vk::DescriptorType::eStorageBuffer;
    pool_sizes[1].descriptorCount = 2 + MaxVertexBlendingMeshCount; // global ring buffer + null buffer
    pool_sizes[2].type            = vk::DescriptorType::eUniformBuffer;
    pool_sizes[2].descriptorCount = MaxMaterialCount;
    pool_sizes[3].type            = vk::DescriptorType::eCombinedImageSampler;
    pool_sizes[3].descriptorCount = 5 * MaxMaterialCount + 5;
    pool_sizes[4].type            = vk::DescriptorType::eInputAttachment;
    pool_sizes[4].descriptorCount = 10;
    pool_sizes[5].type            = vk::DescriptorType::eUniformBufferDynamic;
    pool_sizes[5].descriptorCount = 3;
    pool_sizes[6].type            = vk::DescriptorType::eStorageImage;
    pool_sizes[6].descriptorCount = 2;

    vk::DescriptorPoolCreateInfo pool_create_info {
        .flags         = {},
        .maxSets       = MaxVertexBlendingMeshCount + MaxVertexBlendingMeshCount + 5,
        .poolSizeCount = pool_sizes.size(),
        .pPoolSizes    = pool_sizes.data(),
    };

    vk::DescriptorPool descriptor_pool = Device->createDescriptorPool(pool_create_info);

    DescriptorPool = new VulkanDescriptorPool(descriptor_pool);
}

void VulkanRHI::CreateSyncObjects()
{
    ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    vk::SemaphoreCreateInfo semaphore_info {};
    vk::FenceCreateInfo     fence_info {.flags = vk::FenceCreateFlagBits::eSignaled};

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        ImageAvailableSemaphores[i] = Device->createSemaphore(semaphore_info);
        RenderFinishedSemaphores[i] = Device->createSemaphore(semaphore_info);
        InFlightFences[i]           = Device->createFence(fence_info);
    }
}

void VulkanRHI::CreateAllocator() {
    VmaVulkanFunctions vulkan_functions {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr   = vkGetDeviceProcAddr,
    };

    VmaAllocatorCreateInfo allocator_create_info {
        .physicalDevice = PhysicalDevice,
        .device = Device,
        .pVulkanFunctions = &vulkan_functions,
        .instance = Instance,
        .vulkanApiVersion = VK_API_VERSION_1_3,
    };
    
    vmaCreateAllocator(&allocator_create_info, &Allocator);
}

void VulkanRHI::CreateColorResources()
{
    vk::Format format = SwapChainImageFormat;

    CreateImage(SwapChainExtent.width,
                SwapChainExtent.height,
                format,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                ColorImage,
                ColorImageMemory,
                {},
                1,
                1,
                MsaaSamples);

    CreateImageView(ColorImage, format, vk::ImageAspectFlagBits::eColor, vk::ImageViewType::e2D, 1, 1, ColorImageView);
}

void VulkanRHI::CreateDepthResources()
{
    CreateImage(SwapChainExtent.width,
                SwapChainExtent.height,
                DepthImageFormat,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                DepthImage,
                DepthImageMemory,
                {},
                1,
                1,
                MsaaSamples);

    CreateImageView(DepthImage, DepthImageFormat, vk::ImageAspectFlagBits::eDepth, vk::ImageViewType::e2D, 1, 1, DepthImageView);

    //VulkanUtil::TransitionImageLayout(this,
    //                                  *(VulkanImage*)DepthImage,
    //                                  DepthImageFormat,
    //                                  vk::ImageLayout::eUndefined,
    //                                  vk::ImageLayout::eDepthStencilAttachmentOptimal,
    //                                  vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
}

bool VulkanRHI::CheckValidationLayerSupport()
{
    std::vector<vk::LayerProperties> available_layers = vk::enumerateInstanceLayerProperties();

    for (const char* layer_name : ValidationLayers)
    {
        if (std::none_of(available_layers.begin(), available_layers.end(), [=](const auto& layer_prop) {
                return strcmp(layer_name, layer_prop.layerName) == 0;
            }))
        {
            return false;
        }
    }

    return true;
}

std::vector<const char*> VulkanRHI::GetRequiredExtensions()
{
    uint32_t     glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (EnableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool VulkanRHI::IsDeviceSuitable(vk::PhysicalDevice device)
{
    vk::PhysicalDeviceProperties properties = device.getProperties();
    vk::PhysicalDeviceFeatures   features   = device.getFeatures();
    QueueFamilyIndices           indices    = FindQueueFamilies(device);

    bool is_extensions_supported = IsDeviceExtensionSupport(device);
    bool is_swapchain_adequate   = false;

    if (is_extensions_supported)
    {
        SwapChainSupportDetails details = QuerySwapChainSupport(device);
        is_swapchain_adequate           = !details.Formats.empty() && !details.PresentModes.empty();
    }

    return features.geometryShader &&
           indices.isComplete() && is_extensions_supported && is_swapchain_adequate && features.samplerAnisotropy;
}

QueueFamilyIndices VulkanRHI::FindQueueFamilies(vk::PhysicalDevice physical_device)
{
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = physical_device.getQueueFamilyProperties();

    int i = 0;
    for (const auto& family : queueFamilies)
    {
        if (family.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.GraphicsFamily = i;
        }

        if (family.queueFlags & vk::QueueFlagBits::eCompute)
        {
            indices.ComputeFamily = i;
        }

        if (physical_device.getSurfaceSupportKHR(i, Surface))
        {
            indices.PresentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

bool VulkanRHI::IsDeviceExtensionSupport(vk::PhysicalDevice device)
{
    std::vector<vk::ExtensionProperties> available_extensions = device.enumerateDeviceExtensionProperties();

    std::set<std::string> required_extensions(DeviceExtensions.begin(), DeviceExtensions.end());
    for (const auto& extension : available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

SwapChainSupportDetails VulkanRHI::QuerySwapChainSupport(vk::PhysicalDevice device)
{
    SwapChainSupportDetails details;

    details.Capabilities = device.getSurfaceCapabilitiesKHR(Surface);
    details.PresentModes = device.getSurfacePresentModesKHR(Surface);
    details.Formats      = device.getSurfaceFormatsKHR(Surface);

    return details;
}

vk::SurfaceFormatKHR VulkanRHI::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats)
{
    for (const auto& available_format : available_formats)
    {
        if (available_format.format == vk::Format::eB8G8R8A8Srgb &&
            available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return available_format;
        }
    }

    return available_formats[0];
}

vk::PresentModeKHR VulkanRHI::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes)
{
    for (const auto& available_present_mode : available_present_modes)
    {
        if (available_present_mode == vk::PresentModeKHR::eMailbox)
        {
            return available_present_mode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanRHI::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    int width, height;
    glfwGetFramebufferSize(Window, &width, &height);

    vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}
