//#define GLM_FORCE_LEFT_HANDED

//#include "platform/file/file_manager.h"
//#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.hpp>
//#include <optional>
//#include <set>
//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
//
//const int MAX_FRAMES_IN_FLIGHT = 2;
//
//vk::DispatchLoaderDynamic dispatch_dynamic;
//
//const std::vector<const char*> validation_layers {"VK_LAYER_KHRONOS_validation"};
//
//#ifdef NDEBUG
//const bool enable_validation_layers = false;
//#else
//const bool enable_validation_layers = true;
//#endif // DEBUG
//
//GLFWwindow*                window;
//
//vk::DebugUtilsMessengerEXT debug_messenger;
//
//vk::Instance       instance;
//vk::PhysicalDevice physical_device;
//vk::Device         device;
//
//vk::SurfaceKHR   surface;
//vk::SwapchainKHR swap_chain;
//
//vk::Queue graphics_queue;
//vk::Queue compute_queue;
//vk::Queue present_queue;
//
//std::vector<vk::Image>     swap_chain_images;
//vk::Format                 swap_chain_image_format;
//vk::Extent2D               swap_chain_extent;
//std::vector<vk::ImageView> swap_chain_image_views;
//
//vk::Image        color_image;
//vk::DeviceMemory color_image_memory;
//vk::ImageView    color_image_view;
//
//vk::DescriptorSetLayout        descriptor_set_layout;
//vk::DescriptorPool             descriptor_pool;
//std::vector<vk::DescriptorSet> descriptor_sets;
//
//vk::RenderPass     render_pass;
//vk::PipelineLayout pipeline_layout;
//
//vk::PipelineCache pipeline_cache;
//vk::Pipeline      graphics_pipeline;
//    //vk::ShaderModule frag_shader;
////vk::ShaderModule vert_shader;
//
//std::vector<vk::Framebuffer> swap_chain_frame_buffers;
//
//vk::CommandPool command_pool;
//
//std::vector<vk::CommandBuffer> command_buffers;
//
//std::vector<vk::Semaphore> image_available_semaphores;
//std::vector<vk::Semaphore> render_finished_semaphores;
//std::vector<vk::Fence>     in_flight_fences;
//
//uint32_t current_frame        = 0;
//bool     frame_buffer_resized = false;
//
//struct QueueFamilyIndices
//{
//    std::optional<uint32_t> m_graphics_family;
//    std::optional<uint32_t> m_present_family;
//    std::optional<uint32_t> m_compute_family;
//
//    bool isComplete()
//    {
//        return m_graphics_family.has_value() && m_present_family.has_value() && m_compute_family.has_value();
//        ;
//    }
//};
//
//struct SwapChainSupportDetails
//{
//    vk::SurfaceCapabilitiesKHR        capabilities;
//    std::vector<vk::SurfaceFormatKHR> formats;
//    std::vector<vk::PresentModeKHR>   presentModes;
//};
//
//struct Vertex
//{
//    glm::vec3 pos;
//    glm::vec3 color;
//    glm::vec2 tex_coord;
//
//    static vk::VertexInputBindingDescription GetBindingDescription() {
//        vk::VertexInputBindingDescription binding_description{
//            .binding = 0,
//            .stride = sizeof(Vertex),
//            .inputRate = vk::VertexInputRate::eVertex
//        };
//
//        return binding_description;
//    }
//
//    static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions() {
//        std::array<vk::VertexInputAttributeDescription, 3> attribute_descriptions;
//
//        attribute_descriptions[0] = {
//            .location = 0,
//            .binding = 0,
//            .format = vk::Format::eR32G32B32Sfloat,
//            .offset = offsetof(Vertex, pos)
//        };
//
//        attribute_descriptions[1] = {
//            .location = 1,
//            .binding = 0,
//            .format = vk::Format::eR32G32B32Sfloat,
//            .offset = offsetof(Vertex, color)
//        };
//        
//        attribute_descriptions[2] = {
//            .location = 2,
//            .binding = 0,
//            .format = vk::Format::eR32G32Sfloat,
//            .offset = offsetof(Vertex, tex_coord)
//        };
//
//        return attribute_descriptions;
//    }
//
//};
//
//struct UniformBufferObject
//{
//    glm::mat4 model;
//    glm::mat4 view;
//    glm::mat4 projection;
//};
//
//std::vector<Vertex>   vertices;
//std::vector<uint32_t> indices;
//
//vk::Buffer       vertex_buffer;
//vk::DeviceMemory vertex_buffer_memory;
//
//vk::Buffer       index_buffer;
//vk::DeviceMemory index_buffer_memory;
//
//std::vector<vk::Buffer>       uniform_buffers;
//std::vector<vk::DeviceMemory> uniform_buffers_memory;
//std::vector<void*>            uniform_buffers_mapped;
//
//uint32_t         mip_levels;
//vk::Image        texture_image;
//vk::DeviceMemory texture_image_memory;
//
//vk::ImageView texture_image_view;
//vk::Sampler   texture_sampler;
//
//vk::Image        depth_image;
//vk::ImageView    depth_image_view;
//vk::DeviceMemory depth_image_memory;
//
//vk::SampleCountFlagBits msaa_samples = vk::SampleCountFlagBits::e1;
//
//uint32_t FindMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags properties)
//{
//    vk::PhysicalDeviceMemoryProperties memory_properties = physical_device.getMemoryProperties();
//
//    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
//    {
//        if ((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
//            return i;
//    }
//
//    throw std::runtime_error("failed to find suitable memory type!");
//}
//
//vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
//    for (vk::Format format : candidates)
//    {
//        vk::FormatProperties properties = physical_device.getFormatProperties(format);
//
//        if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
//        {
//            return format;
//        }
//
//        if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
//        {
//            return format;
//        }
//    }
//
//    throw std::runtime_error("failed to find supported format");
//}
//
//vk::Format FindDepthFormat() {
//    return FindSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
//                               vk::ImageTiling::eOptimal,
//                               vk::FormatFeatureFlagBits::eDepthStencilAttachment);
//}
//
//bool HasStencilComponent(vk::Format format) {
//    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
//}
//
//vk::SampleCountFlagBits GetMaxUsableSampleCount()
//{
//    vk::PhysicalDeviceProperties physical_device_properties = physical_device.getProperties();
//
//    vk::SampleCountFlags counts = physical_device_properties.limits.framebufferColorSampleCounts &
//                                  physical_device_properties.limits.framebufferDepthSampleCounts;
//
//    if (counts & vk::SampleCountFlagBits::e64)
//        return vk::SampleCountFlagBits::e64;
//    if (counts & vk::SampleCountFlagBits::e32)
//        return vk::SampleCountFlagBits::e32;
//    if (counts & vk::SampleCountFlagBits::e16)
//        return vk::SampleCountFlagBits::e16;
//    if (counts & vk::SampleCountFlagBits::e8)
//        return vk::SampleCountFlagBits::e8;
//    if (counts & vk::SampleCountFlagBits::e4)
//        return vk::SampleCountFlagBits::e4;
//    if (counts & vk::SampleCountFlagBits::e2)
//        return vk::SampleCountFlagBits::e2;
//
//    return vk::SampleCountFlagBits::e1;
//}
//
//vk::CommandBuffer BeginSingleTimeCommands()
//{
//    vk::CommandBufferAllocateInfo alloc_info {
//        .commandPool        = command_pool,
//        .level              = vk::CommandBufferLevel::ePrimary,
//        .commandBufferCount = 1,
//    };
//
//    vk::CommandBuffer command_buffer = device.allocateCommandBuffers(alloc_info)[0];
//
//    vk::CommandBufferBeginInfo begin_info {
//        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
//    };
//
//    command_buffer.begin(begin_info);
//
//    return command_buffer;
//}
//
//void EndSingleTimeCommands(vk::CommandBuffer command_buffer)
//{
//    command_buffer.end();
//
//    vk::SubmitInfo submit_info {
//        .commandBufferCount = 1,
//        .pCommandBuffers    = &command_buffer,
//    };
//
//    graphics_queue.submit({submit_info});
//    graphics_queue.waitIdle();
//
//    device.freeCommandBuffers(command_pool, {command_buffer});
//}
//
//void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& buffer_memory) {
//    vk::BufferCreateInfo buffer_info {
//        .flags       = {},
//        .size        = size,
//        .usage       = usage,
//        .sharingMode = vk::SharingMode::eExclusive,
//    };
//
//    buffer = device.createBuffer(buffer_info);
//
//    vk::MemoryRequirements memory_requirements = device.getBufferMemoryRequirements(buffer);
//
//    vk::MemoryAllocateInfo alloc_info {
//        .allocationSize = memory_requirements.size,
//        .memoryTypeIndex =
//            FindMemoryType(memory_requirements.memoryTypeBits, properties)};
//
//    buffer_memory = device.allocateMemory(alloc_info);
//
//    device.bindBufferMemory(buffer, buffer_memory, 0);
//}
//
//void CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size) {
//    vk::CommandBuffer command_buffer = BeginSingleTimeCommands();
//
//    vk::BufferCopy copy_region {
//        .srcOffset = 0,
//        .dstOffset = 0,
//        .size      = size,
//    };
//
//    command_buffer.copyBuffer(src, dst, {copy_region});
//
//    EndSingleTimeCommands(command_buffer);
//}
//
//void CreateImage(uint32_t                width,
//                 uint32_t                height,
//                 uint32_t                mip_levels,
//                 vk::SampleCountFlagBits sample_count,
//                 vk::Format              format,
//                 vk::ImageTiling         tiling,
//                 vk::ImageUsageFlags     usage,
//                 vk::MemoryPropertyFlags properties,
//                 vk::Image&              image,
//                 vk::DeviceMemory&       image_memory)
//{
//    vk::ImageCreateInfo image_info {
//        .imageType     = vk::ImageType::e2D,
//        .format        = format,
//        .extent        = {width, height, 1},
//        .mipLevels     = mip_levels,
//        .arrayLayers   = 1,
//        .samples       = sample_count,
//        .tiling        = tiling,
//        .usage         = usage,
//        .sharingMode   = vk::SharingMode::eExclusive,
//        .initialLayout = vk::ImageLayout::eUndefined,
//    };
//
//    image = device.createImage(image_info);
//
//    vk::MemoryRequirements memory_requirements = device.getImageMemoryRequirements(image);
//
//    vk::MemoryAllocateInfo alloc_info {
//        .allocationSize  = memory_requirements.size,
//        .memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, properties),
//    };
//
//    image_memory = device.allocateMemory(alloc_info);
//
//    device.bindImageMemory(image, image_memory, 0);
//}
//
//vk::ImageView CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect_flags, uint32_t mip_levels)
//{
//    vk::ImageSubresourceRange subresource_range {
//        .aspectMask     = aspect_flags,
//        .baseMipLevel   = 0,
//        .levelCount     = mip_levels,
//        .baseArrayLayer = 0,
//        .layerCount     = 1,
//    };
//
//    vk::ImageViewCreateInfo view_info {
//        .image            = image,
//        .viewType         = vk::ImageViewType::e2D,
//        .format           = format,
//        .subresourceRange = subresource_range,
//    };
//
//    return device.createImageView(view_info);
//}
//
//void TransitionImageLayout(vk::Image       image,
//                           vk::Format      format,
//                           vk::ImageLayout old_layout,
//                           vk::ImageLayout new_layout,
//                           uint32_t        mip_levels)
//{
//    vk::CommandBuffer command_buffer = BeginSingleTimeCommands();
//
//    vk::ImageSubresourceRange sub_resource_range {
//        .baseMipLevel   = 0,
//        .levelCount     = mip_levels,
//        .baseArrayLayer = 0,
//        .layerCount     = 1,
//    };
//
//    if (new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
//    {
//        sub_resource_range.aspectMask = vk::ImageAspectFlagBits::eDepth;
//        if (HasStencilComponent(format))
//        {
//            sub_resource_range.aspectMask |= vk::ImageAspectFlagBits::eStencil;
//        }
//    }
//    else
//    {
//        sub_resource_range.aspectMask = vk::ImageAspectFlagBits::eColor;
//    }
//
//    vk::ImageMemoryBarrier barrier {
//        .oldLayout           = old_layout,
//        .newLayout           = new_layout,
//        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//        .image               = image,
//        .subresourceRange    = sub_resource_range,
//    };
//
//    vk::PipelineStageFlags source_stage;
//    vk::PipelineStageFlags destination_stage;
//
//    if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal)
//    {
//        barrier.srcAccessMask = {};
//        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
//
//        source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
//        destination_stage = vk::PipelineStageFlagBits::eTransfer;
//    }
//    else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
//    {
//        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
//        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
//
//        source_stage      = vk::PipelineStageFlagBits::eTransfer;
//        destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
//    }
//    else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
//    {
//        barrier.srcAccessMask = {};
//        barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
//
//        source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
//        destination_stage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
//    }
//    else
//    {
//        throw std::invalid_argument("unsupported layout transition!");
//    }
//
//    command_buffer.pipelineBarrier(source_stage, destination_stage, {}, {}, {}, {barrier});
//
//    EndSingleTimeCommands(command_buffer);
//}
//
//void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
//{
//    vk::CommandBuffer command_buffer = BeginSingleTimeCommands();
//
//    vk::ImageSubresourceLayers image_subresource {
//        .aspectMask = vk::ImageAspectFlagBits::eColor,
//        .mipLevel = 0,
//        .baseArrayLayer = 0,
//        .layerCount = 1,
//    };
//
//    vk::BufferImageCopy region {
//        .bufferOffset = 0,
//        .bufferRowLength = 0,
//        .bufferImageHeight = 0,
//        .imageSubresource = image_subresource,
//        .imageOffset = {0,0,0},
//        .imageExtent = {width, height, 1},
//    };
//
//    command_buffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {region});
//
//    EndSingleTimeCommands(command_buffer);
//}
//
//void CreateVertexBuffer()
//{
//    vk::DeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();
//
//    vk::Buffer       staging_buffer;
//    vk::DeviceMemory staging_buffer_memory;
//    CreateBuffer(buffer_size,
//                 vk::BufferUsageFlagBits::eTransferSrc,
//                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
//                 staging_buffer,
//                 staging_buffer_memory);
//
//    void* data = device.mapMemory(staging_buffer_memory, 0, buffer_size);
//    memcpy(data, vertices.data(), buffer_size);
//    device.unmapMemory(staging_buffer_memory);
//
//    CreateBuffer(buffer_size,
//                 vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
//                 vk::MemoryPropertyFlagBits::eDeviceLocal,
//                 vertex_buffer,
//                 vertex_buffer_memory);
//
//    CopyBuffer(staging_buffer, vertex_buffer, buffer_size);
//
//    device.destroyBuffer(staging_buffer);
//    device.free(staging_buffer_memory);
//}
//
//void CreateIndexBuffer() {
//    vk::DeviceSize buffer_size = sizeof(indices[0]) * indices.size();
//
//    vk::Buffer       staging_buffer;
//    vk::DeviceMemory staging_buffer_memory;
//    CreateBuffer(buffer_size,
//                 vk::BufferUsageFlagBits::eTransferSrc,
//                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
//                 staging_buffer,
//                 staging_buffer_memory);
//
//    void* data = device.mapMemory(staging_buffer_memory, 0, buffer_size);
//    memcpy(data, indices.data(), buffer_size);
//    device.unmapMemory(staging_buffer_memory);
//
//    CreateBuffer(buffer_size,
//                 vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
//                 vk::MemoryPropertyFlagBits::eDeviceLocal,
//                 index_buffer,
//                 index_buffer_memory);
//
//    CopyBuffer(staging_buffer, index_buffer, buffer_size);
//
//    device.destroyBuffer(staging_buffer);
//    device.free(staging_buffer_memory);
//}
//
//void CreateUniformBuffers() { vk::DeviceSize buffer_size = sizeof(UniformBufferObject);
//    uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
//    uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
//    uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);
//
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//    {
//        CreateBuffer(buffer_size,
//                     vk::BufferUsageFlagBits::eUniformBuffer,
//                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
//                     uniform_buffers[i],
//                     uniform_buffers_memory[i]);
//
//        // persistent mapping
//        uniform_buffers_mapped[i] = device.mapMemory(uniform_buffers_memory[i], 0, buffer_size);
//    }
//}
//
//bool CheckValidationLayerSupport()
//{
//    std::vector<vk::LayerProperties> available_layers = vk::enumerateInstanceLayerProperties();
//
//    for (const char* layer_name : validation_layers)
//    {
//        if (std::none_of(available_layers.begin(), available_layers.end(), [=](const auto& layer_prop) {
//                return strcmp(layer_name, layer_prop.layerName) == 0;
//            }))
//        {
//            return false;
//        }
//    }
//
//    return true;
//}
//
//#include <iostream>
//static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT      message_serverity,
//                                                      vk::DebugUtilsMessageTypeFlagBitsEXT          message_type,
//                                                      const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
//                                                      void*                                         user_data)
//{
//    std::cerr << "validation layer: " << callback_data->pMessage << std::endl;
//    return VK_FALSE;
//}
//
//void SetupDebugMessenger() {
//    if (!enable_validation_layers)
//        return;
//
//    vk::DebugUtilsMessengerCreateInfoEXT create_info {
//        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
//                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
//                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
//        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
//                       vk::DebugUtilsMessageTypeFlagBitsEXT ::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT ::ePerformance,
//        .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)DebugCallback,
//        .pUserData = nullptr
//    };
//
//    debug_messenger = instance.createDebugUtilsMessengerEXT(create_info, nullptr, dispatch_dynamic);
//}
//
//std::vector<const char*> GetRequiredExtensions() {
//
//    uint32_t     glfwExtensionCount = 0;
//    const char** glfwExtensions;
//
//    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//
//    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
//
//    if (enable_validation_layers)
//    {
//        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//    }
//
//    return extensions;
//}
//
//void CreateInstance()
//{
//    if (enable_validation_layers && !CheckValidationLayerSupport())
//    {
//        throw std::runtime_error("validation layers requestd, but not available!");
//    }
//
//    vk::ApplicationInfo appInfo {.pApplicationName   = "Hello Triangle",
//                                 .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
//                                 .pEngineName        = "No Engine",
//                                 .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
//                                 .apiVersion         = VK_API_VERSION_1_3};
//    
//    std::vector<const char*> extensions = GetRequiredExtensions();
//
//    vk::InstanceCreateInfo createInfo {.pApplicationInfo        = &appInfo,
//                                       .enabledLayerCount       = 0,
//                                       .enabledExtensionCount   = (uint32_t)extensions.size(),
//                                       .ppEnabledExtensionNames = extensions.data()};
//
//    if (enable_validation_layers)
//    {
//        createInfo.enabledLayerCount = validation_layers.size();
//        createInfo.ppEnabledLayerNames = validation_layers.data();
//    }
//
//    instance = vk::createInstance(createInfo);
//
//    dispatch_dynamic.init(vkGetInstanceProcAddr);
//    dispatch_dynamic.init(instance);
//}
//
//QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice physical_device)
//{
//    QueueFamilyIndices indices;
//
//    std::vector<vk::QueueFamilyProperties> queueFamilies = physical_device.getQueueFamilyProperties();
//
//    int i = 0;
//    for (const auto& family : queueFamilies)
//    {
//        if (family.queueFlags & vk::QueueFlagBits::eGraphics)
//        {
//            indices.m_graphics_family = i;
//        }
//
//        if (family.queueFlags & vk::QueueFlagBits::eCompute)
//        {
//            indices.m_compute_family = i;
//        }
//
//        if (physical_device.getSurfaceSupportKHR(i, surface))
//        {
//            indices.m_present_family = i;
//        }
//
//        if (indices.isComplete())
//        {
//            break;
//        }
//
//        i++;
//    }
//
//    return indices;
//}
//
//SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device)
//{
//    SwapChainSupportDetails details;
//
//    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
//    details.presentModes = device.getSurfacePresentModesKHR(surface);
//    details.formats      = device.getSurfaceFormatsKHR(surface);
//
//    return details;
//}
//
//bool is_device_extension_support(vk::PhysicalDevice device)
//{
//    std::vector<vk::ExtensionProperties> available_extensions = device.enumerateDeviceExtensionProperties();
//
//    return true;
//}
//
//bool is_device_suitable(vk::PhysicalDevice device)
//{
//    vk::PhysicalDeviceProperties properties = device.getProperties();
//    vk::PhysicalDeviceFeatures   features   = device.getFeatures();
//    QueueFamilyIndices           indices    = FindQueueFamilies(device);
//
//    bool is_extensions_supported = is_device_extension_support(device);
//    bool is_swapchain_adequate   = false;
//
//    if (is_extensions_supported)
//    {
//        SwapChainSupportDetails details = QuerySwapChainSupport(device);
//        is_swapchain_adequate           = !details.formats.empty() && !details.presentModes.empty();
//    }
//
//    return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && features.geometryShader &&
//           indices.isComplete() && is_extensions_supported && is_swapchain_adequate && features.samplerAnisotropy;
//}
//
//void SelectPhysicalDevice()
//{
//    std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
//
//    for (const auto& device : devices)
//    {
//        if (is_device_suitable(device))
//        {
//            physical_device = device;
//            msaa_samples    = GetMaxUsableSampleCount();
//            return;
//        }
//    }
//
//    throw std::runtime_error("failed to find GPUs with Vulkan support!");
//}
//
//void CreateLogicalDevice()
//{
//    QueueFamilyIndices indices = FindQueueFamilies(physical_device);
//
//    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
//    std::set<uint32_t>                     queue_families = {
//                            indices.m_graphics_family.value(), indices.m_compute_family.value(), indices.m_present_family.value()};
//
//    float queue_priority = 1.0f;
//    for (uint32_t queue_family : queue_families)
//    {
//        vk::DeviceQueueCreateInfo info {
//            .queueFamilyIndex = queue_family, .queueCount = 1, .pQueuePriorities = &queue_priority};
//        queue_create_infos.push_back(info);
//    }
//
//    vk::PhysicalDeviceFeatures device_features {
//        .independentBlend = VK_TRUE,
//        .geometryShader = VK_TRUE,
//        .samplerAnisotropy = VK_TRUE,
//        .fragmentStoresAndAtomics = VK_TRUE,
//    };
//
//    std::vector<const char*> device_extensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
//
//    vk::DeviceCreateInfo create_info {.queueCreateInfoCount    = (uint32_t)queue_create_infos.size(),
//                                      .pQueueCreateInfos       = queue_create_infos.data(),
//                                      .enabledLayerCount       = 0,
//                                      .ppEnabledLayerNames     = nullptr,
//                                      .enabledExtensionCount   = (uint32_t)device_extensions.size(),
//                                      .ppEnabledExtensionNames = device_extensions.data(),
//                                      .pEnabledFeatures        = &device_features};
//
//    device = physical_device.createDevice(create_info);
//
//    graphics_queue = device.getQueue(indices.m_graphics_family.value(), 0);
//    compute_queue  = device.getQueue(indices.m_compute_family.value(), 0);
//    present_queue  = device.getQueue(indices.m_present_family.value(), 0);
//
//    dispatch_dynamic.init(device);
//}
//
//vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats) {
//    for (const auto& available_format : available_formats)
//    {
//        if (available_format.format == vk::Format::eB8G8R8A8Srgb &&
//            available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
//        {
//            return available_format;
//        }
//    }
//
//    return available_formats[0];
//}
//
//vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes)
//{
//    for (const auto& available_present_mode : available_present_modes)
//    {
//        if (available_present_mode == vk::PresentModeKHR::eMailbox)
//        {
//            return available_present_mode;
//        }
//    }
//
//    return vk::PresentModeKHR::eFifo;
//}
//
//vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
//{
//    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
//    {
//        return capabilities.currentExtent;
//    }
//    int width, height;
//    glfwGetFramebufferSize(window, &width, &height);
//
//    vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
//
//    actualExtent.width =
//        std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
//    actualExtent.height =
//        std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
//
//    return actualExtent;
//}
//
//void CreateSwapChain() {
//    SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(physical_device);
//
//    vk::SurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
//    vk::PresentModeKHR   present_mode   = ChooseSwapPresentMode(swap_chain_support.presentModes);
//    vk::Extent2D         extent         = ChooseSwapExtent(swap_chain_support.capabilities);
//
//    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
//    if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
//    {
//        image_count = swap_chain_support.capabilities.maxImageCount;
//    }
//
//    vk::SwapchainCreateInfoKHR create_info {
//        .surface = surface,
//        .minImageCount = image_count,
//        .imageFormat = surface_format.format,
//        .imageColorSpace = surface_format.colorSpace,
//        .imageExtent = extent,
//        .imageArrayLayers = 1,
//        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment
//    };
//
//    QueueFamilyIndices indices = FindQueueFamilies(physical_device);
//    uint32_t           queueFamilyIndices[] = {indices.m_graphics_family.value(), indices.m_present_family.value()};
//
//    if (indices.m_graphics_family != indices.m_present_family)
//    {
//        create_info.imageSharingMode      = vk::SharingMode::eConcurrent;
//        create_info.queueFamilyIndexCount = 2;
//        create_info.pQueueFamilyIndices   = queueFamilyIndices;
//    }
//    else
//    {
//        create_info.imageSharingMode      = vk::SharingMode::eExclusive;
//        create_info.queueFamilyIndexCount = 0;       // Optional
//        create_info.pQueueFamilyIndices   = nullptr; // Optional
//    }
//
//    create_info.preTransform   = swap_chain_support.capabilities.currentTransform;
//    create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
//    create_info.presentMode    = present_mode;
//    create_info.clipped        = VK_TRUE;
//    create_info.oldSwapchain   = VK_NULL_HANDLE;
//
//    swap_chain = device.createSwapchainKHR(create_info);
//
//    swap_chain_images       = device.getSwapchainImagesKHR(swap_chain);
//    swap_chain_image_format = surface_format.format;
//    swap_chain_extent       = extent;
//}
//
//void CreateColorResources() {
//    vk::Format format = swap_chain_image_format;
//
//    CreateImage(swap_chain_extent.width,
//                swap_chain_extent.height,
//                1,
//                msaa_samples,
//                format,
//                vk::ImageTiling::eOptimal,
//                vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
//                vk::MemoryPropertyFlagBits::eDeviceLocal,
//                color_image,
//                color_image_memory);
//
//    color_image_view = CreateImageView(color_image, format, vk::ImageAspectFlagBits::eColor, 1);
//}
//
//void CreateImageViews() {
//    swap_chain_image_views.resize(swap_chain_images.size());
//
//    for (size_t i = 0; i < swap_chain_images.size(); i++)
//    {
//        swap_chain_image_views[i] = CreateImageView(swap_chain_images[i], swap_chain_image_format, vk::ImageAspectFlagBits::eColor, 1);
//    }
//}
//
//vk::ShaderModule CreateShaderModule(const std::vector<byte>& code)
//{
//    vk::ShaderModuleCreateInfo create_info {
//        .codeSize = code.size(),
//        .pCode = (const uint32_t*)code.data()
//    };
//
//    vk::ShaderModule shader_module = device.createShaderModule(create_info);
//    return shader_module;
//}
//
//void CreateGraphicsPipeline()
//{
//    std::vector<byte> vert_shader_code = FileManager::Read("D:/Works/YR/ProxyRP/src/shaders/TestVS.spirv");
//    std::vector<byte> frag_shader_code = FileManager::Read("D:/Works/YR/ProxyRP/src/shaders/TestPS.spirv");
//
//    vk::ShaderModule vert_shader = CreateShaderModule(vert_shader_code);
//    vk::ShaderModule frag_shader = CreateShaderModule(frag_shader_code);
//
//    vk::PipelineShaderStageCreateInfo vert_shader_stage_info {
//        .stage = vk::ShaderStageFlagBits::eVertex,
//        .module = vert_shader,
//        .pName = "vert"
//    };
//    
//    vk::PipelineShaderStageCreateInfo frag_shader_stage_info {
//        .stage = vk::ShaderStageFlagBits::eFragment,
//        .module = frag_shader,
//        .pName = "frag"
//    };
//
//    std::vector<vk::PipelineShaderStageCreateInfo> shader_stages {vert_shader_stage_info, frag_shader_stage_info};
//
//    std::vector<vk::DynamicState> dynamic_states {
//        vk::DynamicState::eViewport,
//        vk::DynamicState::eScissor
//    };
//
//    vk::PipelineDynamicStateCreateInfo dynamic_state {
//        .dynamicStateCount = (uint32_t)dynamic_states.size(),
//        .pDynamicStates = dynamic_states.data()
//    };
//
//    vk::VertexInputBindingDescription                  binding_description    = Vertex::GetBindingDescription();
//    std::array<vk::VertexInputAttributeDescription, 3> attribute_descriptions = Vertex::GetAttributeDescriptions();
//
//    vk::PipelineVertexInputStateCreateInfo vertex_input_info {
//        .vertexBindingDescriptionCount   = 1,
//        .pVertexBindingDescriptions      = &binding_description,
//        .vertexAttributeDescriptionCount = (uint32_t)attribute_descriptions.size(),
//        .pVertexAttributeDescriptions    = attribute_descriptions.data(),
//    };
//
//    vk::PipelineInputAssemblyStateCreateInfo input_assembly {
//        .topology = vk::PrimitiveTopology::eTriangleList,
//        .primitiveRestartEnable = VK_FALSE
//    };
//
//    vk::PipelineViewportStateCreateInfo viewport_state {
//        .viewportCount = 1,
//        .pViewports = nullptr,
//        .scissorCount = 1,
//        .pScissors = nullptr
//    };
//
//    vk::PipelineRasterizationStateCreateInfo rasterizer {
//        .depthClampEnable = VK_FALSE,
//        .polygonMode = vk::PolygonMode::eFill,
//        .cullMode = vk::CullModeFlagBits::eBack,
//        .frontFace = vk::FrontFace::eCounterClockwise,
//        .depthBiasEnable = VK_FALSE,
//        .depthBiasConstantFactor = 0.0f,
//        .depthBiasClamp = 0.0f,
//        .depthBiasSlopeFactor = 0.0f,
//        .lineWidth = 1.0f,
//    };
//
//    vk::PipelineMultisampleStateCreateInfo multi_sampling {
//        .rasterizationSamples  = msaa_samples,
//        .sampleShadingEnable   = VK_FALSE,
//        .minSampleShading      = 1.0f,
//        .pSampleMask           = nullptr,
//        .alphaToCoverageEnable = VK_FALSE,
//        .alphaToOneEnable      = VK_FALSE,
//    };
//
//    vk::PipelineColorBlendAttachmentState color_blend_attachment {
//        .blendEnable         = VK_FALSE,
//        .srcColorBlendFactor = vk::BlendFactor::eOne,
//        .dstColorBlendFactor = vk::BlendFactor::eZero,
//        .colorBlendOp        = vk::BlendOp::eAdd,
//        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
//        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
//        .alphaBlendOp        = vk::BlendOp::eAdd,
//        .colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
//                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
//    };
//
//    vk::PipelineColorBlendStateCreateInfo color_blending {.logicOpEnable   = VK_FALSE,
//                                                          .logicOp         = vk::LogicOp::eCopy,
//                                                          .attachmentCount = 1,
//                                                          .pAttachments    = &color_blend_attachment,
//                                                          .blendConstants  = std::array {0.0f, 0.0f, 0.0f, 0.0f}};
//
//    vk::PipelineDepthStencilStateCreateInfo depth_stencil {
//        .depthTestEnable       = VK_TRUE,
//        .depthWriteEnable      = VK_TRUE,
//        .depthCompareOp        = vk::CompareOp::eLess,
//        .depthBoundsTestEnable = VK_FALSE,
//        .stencilTestEnable     = VK_FALSE,
//        .front                 = {},
//        .back                  = {},
//        .minDepthBounds        = 0.0f,
//        .maxDepthBounds        = 1.0f,
//    };
//
//    vk::PipelineLayoutCreateInfo pipeline_layout_info {
//        .setLayoutCount = 1,
//        .pSetLayouts = &descriptor_set_layout,
//        .pushConstantRangeCount = 0,
//        .pPushConstantRanges = nullptr,
//    };
//
//    pipeline_layout = device.createPipelineLayout(pipeline_layout_info);
//
//    vk::GraphicsPipelineCreateInfo pipeline_info {
//        .stageCount = (uint32_t)shader_stages.size(),
//        .pStages    = shader_stages.data(),
//
//        .pVertexInputState   = &vertex_input_info,
//        .pInputAssemblyState = &input_assembly,
//        .pViewportState      = &viewport_state,
//        .pRasterizationState = &rasterizer,
//        .pMultisampleState   = &multi_sampling,
//        .pDepthStencilState  = &depth_stencil,
//        .pColorBlendState    = &color_blending,
//        .pDynamicState       = &dynamic_state,
//
//        .layout = pipeline_layout,
//
//        .renderPass = render_pass,
//        .subpass    = 0,
//
//        .basePipelineHandle = VK_NULL_HANDLE,
//        .basePipelineIndex  = -1,
//    };
//
//    graphics_pipeline = device.createGraphicsPipeline(VK_NULL_HANDLE, pipeline_info).value;
//
//    device.destroyShaderModule(vert_shader);
//    device.destroyShaderModule(frag_shader);
//}
//
//void CreateRenderPass() {
//    vk::AttachmentDescription color_attachment {
//        .format         = swap_chain_image_format,
//        .samples        = msaa_samples,
//        .loadOp         = vk::AttachmentLoadOp::eClear,
//        .storeOp        = vk::AttachmentStoreOp::eStore,
//        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
//        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
//        .initialLayout  = vk::ImageLayout::eUndefined,
//        .finalLayout    = vk::ImageLayout::eColorAttachmentOptimal,
//    };
//
//    vk::AttachmentReference color_attachment_ref {
//        .attachment = 0,
//        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
//    };
//
//    vk::AttachmentDescription color_attachment_resolve {
//        .format         = swap_chain_image_format,
//        .samples        = vk::SampleCountFlagBits::e1,
//        .loadOp         = vk::AttachmentLoadOp::eDontCare,
//        .storeOp        = vk::AttachmentStoreOp::eStore,
//        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
//        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
//        .initialLayout  = vk::ImageLayout::eUndefined,
//        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
//    };
//    
//    vk::AttachmentReference color_attachment_resolve_ref {
//        .attachment = 2,
//        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
//    };
//
//    vk::AttachmentDescription depth_attachment {
//        .format         = FindDepthFormat(),
//        .samples        = msaa_samples,
//        .loadOp         = vk::AttachmentLoadOp::eClear,
//        .storeOp        = vk::AttachmentStoreOp::eDontCare,
//        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
//        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
//        .initialLayout  = vk::ImageLayout::eUndefined,
//        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
//    };
//
//    vk::AttachmentReference depth_attachment_ref {
//        .attachment = 1,
//        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
//    };
//
//    vk::SubpassDependency dependency {
//        .srcSubpass = VK_SUBPASS_EXTERNAL,
//        .dstSubpass = 0,
//        .srcStageMask =
//            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
//        .dstStageMask =
//            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
//        .srcAccessMask = {},
//        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
//    };
//
//    vk::SubpassDescription subpass {
//        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
//        .colorAttachmentCount    = 1,
//        .pColorAttachments       = &color_attachment_ref,
//        .pResolveAttachments     = &color_attachment_resolve_ref,
//        .pDepthStencilAttachment = &depth_attachment_ref,
//    };
//
//    std::array attachments = {color_attachment, depth_attachment, color_attachment_resolve};
//
//    vk::RenderPassCreateInfo render_pass_info {
//        .attachmentCount = (uint32_t)attachments.size(),
//        .pAttachments    = attachments.data(),
//        .subpassCount    = 1,
//        .pSubpasses      = &subpass,
//        .dependencyCount = 1,
//        .pDependencies   = &dependency,
//    };
//
//    render_pass = device.createRenderPass(render_pass_info);
//}
//
//void CreateFrameBuffers() {
//    swap_chain_frame_buffers.resize(swap_chain_image_views.size());
//
//    for (size_t i = 0; i < swap_chain_image_views.size(); i++)
//    {
//        std::array attachments = {color_image_view, depth_image_view, swap_chain_image_views[i]};
//
//        vk::FramebufferCreateInfo frame_buffer_info {
//            .renderPass      = render_pass,
//            .attachmentCount = (uint32_t)attachments.size(),
//            .pAttachments    = attachments.data(),
//            .width           = swap_chain_extent.width,
//            .height          = swap_chain_extent.height,
//            .layers          = 1,
//        };
//
//        swap_chain_frame_buffers[i] = device.createFramebuffer(frame_buffer_info);
//    }
//}
//
//void CreateCommandPool()
//{
//    QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device);
//
//    vk::CommandPoolCreateInfo pool_info {.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
//                                         .queueFamilyIndex = queue_family_indices.m_graphics_family.value()};
//
//    command_pool = device.createCommandPool(pool_info);
//
//}
//
//void CreateCommandBuffer()
//{
//    command_buffers.resize(MAX_FRAMES_IN_FLIGHT);
//
//    vk::CommandBufferAllocateInfo allocate_info {.commandPool        = command_pool,
//                                                 .level              = vk::CommandBufferLevel::ePrimary,
//                                                 .commandBufferCount = (uint32_t)command_buffers.size()};
//
//    command_buffers = device.allocateCommandBuffers(allocate_info);
//}
//
//void RecordCommandBuffer(vk::CommandBuffer command_buffer, uint32_t image_index) {
//    vk::CommandBufferBeginInfo begin_info {
//        .flags            = {},
//        .pInheritanceInfo = nullptr,
//    };
//
//    command_buffer.begin(begin_info);
//
//    std::array<vk::ClearValue, 2> clear_colors {};
//    clear_colors[0].color        = vk::ClearColorValue {std::array {0.0f, 0.0f, 0.0f, 1.0f}};
//    clear_colors[1].depthStencil = vk::ClearDepthStencilValue {1.0f, 0};
//
//    vk::RenderPassBeginInfo render_pass_info {
//        .renderPass      = render_pass,
//        .framebuffer     = swap_chain_frame_buffers[image_index],
//        .renderArea      = {0, 0, swap_chain_extent},
//        .clearValueCount = (uint32_t)clear_colors.size(),
//        .pClearValues    = clear_colors.data(),
//    };
//
//    command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
//    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline);
//
//    vk::Buffer     vertex_buffers[] = {vertex_buffer};
//    vk::DeviceSize offsets[]        = {0};
//
//    command_buffer.bindVertexBuffers(0, vertex_buffers, offsets);
//
//    command_buffer.bindIndexBuffer(index_buffer, 0, vk::IndexType::eUint32);
//
//    command_buffer.bindDescriptorSets(
//        vk::PipelineBindPoint::eGraphics, pipeline_layout, 0, {descriptor_sets[current_frame]}, {});
//
//    vk::Viewport viewport {
//        .x        = 0.0f,
//        .y        = 0.0f,
//        .width    = (float)swap_chain_extent.width,
//        .height   = (float)swap_chain_extent.height,
//        .minDepth = 0.0f,
//        .maxDepth = 1.0f,
//    };
//
//    vk::Rect2D scissor {0, 0, swap_chain_extent};
//
//    command_buffer.setViewport(0, 1, &viewport);
//    command_buffer.setScissor(0, 1, &scissor);
//
//    //command_buffer.draw((uint32_t)vertices.size(), 1, 0, 0);
//    command_buffer.drawIndexed(indices.size(), 1, 0, 0, 0);
//
//    command_buffer.endRenderPass();
//    command_buffer.end();
//}
//
//void UpdateUniformBuffer(uint32_t current_image) {
//    static auto start_time = std::chrono::high_resolution_clock::now();
//
//    auto current_time = std::chrono::high_resolution_clock::now();
//    float time         = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
//
//    UniformBufferObject ubo {
//        .model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
//        .view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
//        .projection = glm::perspective(
//            glm::radians(45.0f), swap_chain_extent.width / (float)swap_chain_extent.height, 0.1f, 10.0f),
//    };
//    ubo.projection[1][1] *= -1;
//    //ubo.model = glm::transpose(ubo.model);
//    //ubo.view = glm::transpose(ubo.view);
//    //ubo.projection = glm::transpose(ubo.projection);
//
//    memcpy(uniform_buffers_mapped[current_image], &ubo, sizeof(ubo));
//}
//
//void CreateDescriptorSetLayout()
//{
//    vk::DescriptorSetLayoutBinding ubo_layout_binding {
//        .binding            = 0,
//        .descriptorType     = vk::DescriptorType::eUniformBuffer,
//        .descriptorCount    = 1,
//        .stageFlags         = vk::ShaderStageFlagBits::eVertex,
//        .pImmutableSamplers = nullptr,
//    };
//
//    vk::DescriptorSetLayoutBinding sampler_layout_binding {
//        .binding            = 1,
//        .descriptorType     = vk::DescriptorType::eCombinedImageSampler,
//        .descriptorCount    = 1,
//        .stageFlags         = vk::ShaderStageFlagBits::eFragment,
//        .pImmutableSamplers = nullptr,
//    };
//
//    std::array bindings = {ubo_layout_binding, sampler_layout_binding};
//
//    vk::DescriptorSetLayoutCreateInfo layout_info {
//        .bindingCount = (uint32_t)bindings.size(),
//        .pBindings    = bindings.data(),
//    };
//
//    descriptor_set_layout = device.createDescriptorSetLayout(layout_info);
//}
//
//void CreateDescriptorPool() {
//    vk::DescriptorPoolSize ub_pool_size {
//        .type            = vk::DescriptorType::eUniformBuffer,
//        .descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT,
//    };
//
//    vk::DescriptorPoolSize cis_pool_size {
//        .type            = vk::DescriptorType::eCombinedImageSampler,
//        .descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT,
//    };
//
//    std::array pool_sizes = {ub_pool_size, cis_pool_size};
//
//    vk::DescriptorPoolCreateInfo pool_info {
//        .maxSets       = (uint32_t)MAX_FRAMES_IN_FLIGHT,
//        .poolSizeCount = (uint32_t)pool_sizes.size(),
//        .pPoolSizes    = pool_sizes.data(),
//    };
//
//    descriptor_pool = device.createDescriptorPool(pool_info);
//}
//
//void CreateDescriptorSets() {
//    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptor_set_layout);
//
//    vk::DescriptorSetAllocateInfo alloc_info {
//        .descriptorPool     = descriptor_pool,
//        .descriptorSetCount = (uint32_t)MAX_FRAMES_IN_FLIGHT,
//        .pSetLayouts        = layouts.data(),
//    };
//
//    descriptor_sets = device.allocateDescriptorSets(alloc_info);
//
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//    {
//        vk::DescriptorBufferInfo buffer_info {
//            .buffer = uniform_buffers[i],
//            .offset = 0,
//            .range  = sizeof(UniformBufferObject),
//        };
//
//        vk::DescriptorImageInfo image_info {
//            .sampler     = texture_sampler,
//            .imageView   = texture_image_view,
//            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
//        };
//
//        vk::WriteDescriptorSet ub_descriptor_write {
//            .dstSet           = descriptor_sets[i],
//            .dstBinding       = 0,
//            .dstArrayElement  = 0,
//            .descriptorCount  = 1,
//            .descriptorType   = vk::DescriptorType::eUniformBuffer,
//            .pImageInfo       = nullptr,
//            .pBufferInfo      = &buffer_info,
//            .pTexelBufferView = nullptr,
//        };
//
//        vk::WriteDescriptorSet cis_descriptor_write {
//            .dstSet           = descriptor_sets[i],
//            .dstBinding       = 1,
//            .dstArrayElement  = 0,
//            .descriptorCount  = 1,
//            .descriptorType   = vk::DescriptorType::eCombinedImageSampler,
//            .pImageInfo       = &image_info,
//            .pBufferInfo      = nullptr,
//            .pTexelBufferView = nullptr,
//        };
//
//        device.updateDescriptorSets({ub_descriptor_write, cis_descriptor_write}, {});
//    }
//}
//
//void CreateSyncObjects()
//{
//    image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
//    render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
//    in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
//
//    vk::SemaphoreCreateInfo semaphore_info {};
//    vk::FenceCreateInfo     fence_info {
//        .flags = vk::FenceCreateFlagBits::eSignaled
//    };
//
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//    {
//        image_available_semaphores[i] = device.createSemaphore(semaphore_info);
//        render_finished_semaphores[i] = device.createSemaphore(semaphore_info);
//        in_flight_fences[i]           = device.createFence(fence_info);
//    }
//}
//
//void CreateDepthResources() {
//    vk::Format depth_format = FindDepthFormat();
//
//    CreateImage(swap_chain_extent.width,
//                swap_chain_extent.height,
//                1,
//                msaa_samples,
//                depth_format,
//                vk::ImageTiling::eOptimal,
//                vk::ImageUsageFlagBits::eDepthStencilAttachment,
//                vk::MemoryPropertyFlagBits::eDeviceLocal,
//                depth_image,
//                depth_image_memory);
//
//    depth_image_view = CreateImageView(depth_image, depth_format, vk::ImageAspectFlagBits::eDepth, 1);
//
//    TransitionImageLayout(
//        depth_image, depth_format, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, 1);
//
//}
//
//void GenerateMipmaps(vk::Image image, vk::Format format, uint32_t width, uint32_t height, uint32_t mip_levels) {
//    vk::FormatProperties format_properties = physical_device.getFormatProperties(format);
//    if (!(format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
//    {
//        throw std::runtime_error("texture image format does not support linear blitting!");
//    }
//
//    vk::CommandBuffer command_buffer = BeginSingleTimeCommands();
//
//    vk::ImageSubresourceRange sub_resource_range {
//        .aspectMask     = vk::ImageAspectFlagBits::eColor,
//        .levelCount     = 1,
//        .baseArrayLayer = 0,
//        .layerCount     = 1,
//    };
//
//    vk::ImageMemoryBarrier barrier {
//        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//        .image               = image,
//        .subresourceRange    = sub_resource_range,
//    };
//
//    int32_t mip_width  = width;
//    int32_t mip_height = height;
//    for (size_t i = 1; i < mip_levels; i++)
//    {
//        barrier.subresourceRange.baseMipLevel = i - 1;
//        barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
//        barrier.newLayout                     = vk::ImageLayout::eTransferSrcOptimal;
//        barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
//        barrier.dstAccessMask                 = vk::AccessFlagBits::eTransferRead;
//        
//        command_buffer.pipelineBarrier(
//            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, {barrier});
//
//        vk::ImageBlit blit;
//        blit.srcOffsets[0]                 = vk::Offset3D {0, 0, 0};
//        blit.srcOffsets[1]                 = vk::Offset3D {mip_width, mip_height, 1};
//        blit.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
//        blit.srcSubresource.mipLevel       = i - 1;
//        blit.srcSubresource.baseArrayLayer = 0;
//        blit.srcSubresource.layerCount     = 1;
//
//        blit.dstOffsets[0]                 = vk::Offset3D {0, 0, 0};
//        blit.dstOffsets[1]                 = vk::Offset3D {std::max(mip_width / 2, 1), std::max(mip_height / 2, 1), 1};
//        blit.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
//        blit.dstSubresource.mipLevel       = i;
//        blit.dstSubresource.baseArrayLayer = 0;
//        blit.dstSubresource.layerCount     = 1;
//
//        command_buffer.blitImage(image,
//                                 vk::ImageLayout::eTransferSrcOptimal,
//                                 image,
//                                 vk::ImageLayout::eTransferDstOptimal,
//                                 {blit},
//                                 vk::Filter::eLinear);
//
//        barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
//        barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
//        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
//        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
//
//        command_buffer.pipelineBarrier(
//            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {barrier});
//
//        mip_width  = std::max(mip_width / 2, 1);
//        mip_height = std::max(mip_height / 2, 1);
//    }
//
//    barrier.subresourceRange.baseMipLevel = mip_levels - 1;
//    barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
//    barrier.newLayout                     = vk::ImageLayout::eShaderReadOnlyOptimal;
//    barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
//    barrier.dstAccessMask                 = vk::AccessFlagBits::eShaderRead;
//
//    command_buffer.pipelineBarrier(
//        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {barrier});
//
//    EndSingleTimeCommands(command_buffer);
//}
//
//void CreateTextureImage()
//{
//    auto texture = TextureLoader::LoadData(TEXT("D:/Works/YR/ProxyRP/src/temp/viking_room.png"));
//
//    mip_levels = texture->GetMipLevels();
//
//    vk::DeviceSize image_size = texture->GetSize();
//
//    vk::Buffer       staging_buffer;
//    vk::DeviceMemory staging_buffer_memory;
//
//    CreateBuffer(image_size,
//                 vk::BufferUsageFlagBits::eTransferSrc,
//                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
//                 staging_buffer,
//                 staging_buffer_memory);
//
//    void* data = device.mapMemory(staging_buffer_memory, 0, image_size);
//    memcpy(data, texture->GetData(), texture->GetSize());
//    device.unmapMemory(staging_buffer_memory);
//
//    CreateImage(texture->Width,
//                texture->Height,
//                mip_levels,
//                vk::SampleCountFlagBits::e1,
//                vk::Format::eR8G8B8A8Srgb,
//                vk::ImageTiling::eOptimal,
//                vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
//                    vk::ImageUsageFlagBits::eSampled,
//                vk::MemoryPropertyFlagBits::eDeviceLocal,
//                texture_image,
//                texture_image_memory);
//
//    TransitionImageLayout(
//        texture_image, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, mip_levels);
//
//    CopyBufferToImage(staging_buffer, texture_image, texture->Width, texture->Height);
//
//    GenerateMipmaps(texture_image, vk::Format::eR8G8B8A8Srgb, texture->Width, texture->Height, mip_levels);
//
//    //TransitionImageLayout(texture_image,
//    //                      vk::Format::eR8G8B8A8Srgb,
//    //                      vk::ImageLayout::eTransferDstOptimal,
//    //                      vk::ImageLayout::eShaderReadOnlyOptimal,
//    //                      mip_levels);
//
//    device.destroyBuffer(staging_buffer);
//    device.freeMemory(staging_buffer_memory);
//}
//
//void CreateTextureImageView() {
//    texture_image_view = CreateImageView(texture_image, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, mip_levels);
//}
//
//void CreateTextureSampler() {
//    vk::PhysicalDeviceProperties properties = physical_device.getProperties();
//
//    vk::SamplerCreateInfo sampler_info {
//        .magFilter               = vk::Filter::eLinear,
//        .minFilter               = vk::Filter::eLinear,
//        .mipmapMode              = vk::SamplerMipmapMode::eLinear,
//        .addressModeU            = vk::SamplerAddressMode::eRepeat,
//        .addressModeV            = vk::SamplerAddressMode::eRepeat,
//        .addressModeW            = vk::SamplerAddressMode::eRepeat,
//        .mipLodBias              = 0.0f,
//        .anisotropyEnable        = VK_TRUE,
//        .maxAnisotropy           = properties.limits.maxSamplerAnisotropy,
//        .compareEnable           = VK_FALSE,
//        .compareOp               = vk::CompareOp::eAlways,
//        .minLod                  = 0.0f,
//        .maxLod                  = (float)mip_levels,
//        .borderColor             = vk::BorderColor::eIntOpaqueBlack,
//        .unnormalizedCoordinates = VK_FALSE,
//    };
//
//    texture_sampler = device.createSampler(sampler_info);
//}
//
//void LoadModel() {
//    Model model = ModelLoader::Load(TEXT("D:/Works/YR/ProxyRP/src/temp/viking_room.obj"));
//
//    for (auto& mesh : model.Meshes)
//    {
//        for (auto& raw : mesh.Vertices)
//        {
//            Vertex vertex;
//            vertex.pos       = raw.Position;
//            vertex.tex_coord = {raw.TexCoords.x, 1.0f - raw.TexCoords.y};
//            vertices.push_back(vertex);
//        }
//
//        for (uint32_t index : mesh.Indices)
//        {
//            indices.push_back(index);
//        }
//    }
//}
//
//void CleanupSwapChain()
//{
//    device.destroyImageView(color_image_view);
//    device.destroyImage(color_image);
//    device.freeMemory(color_image_memory);
//
//    device.destroyImageView(depth_image_view);
//    device.destroyImage(depth_image);
//    device.freeMemory(depth_image_memory);
//
//    for (auto& frame_buffer : swap_chain_frame_buffers)
//    {
//        device.destroyFramebuffer(frame_buffer);
//    }
//    for (auto& image_view : swap_chain_image_views)
//    {
//        device.destroyImageView(image_view);
//    }
//
//    device.destroySwapchainKHR(swap_chain);
//}
//
//void RecreateSwapChain()
//{
//    int width = 0, height = 0;
//    while (width == 0 || height == 0)
//    {
//        glfwGetFramebufferSize(window, &width, &height);
//        glfwWaitEvents(); 
//    }
//
//    device.waitIdle();
//
//    CleanupSwapChain();
//
//    CreateSwapChain();
//    CreateImageViews();
//    CreateColorResources();
//    CreateDepthResources();
//    CreateFrameBuffers();
//}
//
//void cleanup()
//{
//    CleanupSwapChain();
//
//    device.destroySampler(texture_sampler);
//    device.destroyImageView(texture_image_view);
//    device.destroyImage(texture_image);
//    device.freeMemory(texture_image_memory);
//
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//    {
//        device.destroyBuffer(uniform_buffers[i]);
//        device.freeMemory(uniform_buffers_memory[i]);
//    }
//
//    device.destroyDescriptorPool(descriptor_pool);
//    device.destroyDescriptorSetLayout(descriptor_set_layout);
//
//    device.destroyBuffer(index_buffer);
//    device.freeMemory(index_buffer_memory);
//
//    device.destroyBuffer(vertex_buffer);
//    device.freeMemory(vertex_buffer_memory);
//
//    device.destroyPipeline(graphics_pipeline);
//    device.destroyPipelineLayout(pipeline_layout);
//
//    device.destroyRenderPass(render_pass);
//
//    for (auto& semaphore : image_available_semaphores)
//    {
//        device.destroySemaphore(semaphore);
//    }
//    for (auto& semaphore : render_finished_semaphores)
//    {
//        device.destroySemaphore(semaphore);
//    }
//    for (auto& fence : in_flight_fences)
//    {
//        device.destroyFence(fence);
//    }
//
//    device.destroyCommandPool(command_pool);
//
//    device.destroy();
//
//    if (enable_validation_layers)
//    {
//        instance.destroyDebugUtilsMessengerEXT(debug_messenger, nullptr, dispatch_dynamic);
//    }
//
//    instance.destroySurfaceKHR(surface);
//    instance.destroy();
//
//    glfwDestroyWindow(window);
//    glfwTerminate();
//}
//
//
//void DrawFrame()
//{
//    const vk::Fence&         in_flight_fence           = in_flight_fences[current_frame];
//    const vk::Semaphore&     image_available_semaphore = image_available_semaphores[current_frame];
//    const vk::Semaphore&     render_finished_semaphore = render_finished_semaphores[current_frame];
//    const vk::CommandBuffer& command_buffer            = command_buffers[current_frame];
//
//    vk::Result wait_fence_result = device.waitForFences(1, &in_flight_fence, VK_TRUE, UINT64_MAX);
//
//    vk::ResultValue<uint32_t> image_index =
//        device.acquireNextImageKHR(swap_chain, UINT64_MAX, image_available_semaphore);
//    if (image_index.result == vk::Result::eErrorOutOfDateKHR)
//    {
//        RecreateSwapChain();
//        return;
//    }
//    else if (image_index.result != vk::Result::eSuccess && image_index.result != vk::Result::eSuboptimalKHR)
//    {
//        throw std::runtime_error("failed to acquire swap chain image");
//    }
//
//    vk::Result reset_fence_result = device.resetFences(1, &in_flight_fence);
//
//    command_buffer.reset();
//
//    RecordCommandBuffer(command_buffer, image_index.value);
//
//    UpdateUniformBuffer(current_frame);
//
//    vk::PipelineStageFlags wait_stages[] {vk::PipelineStageFlagBits::eColorAttachmentOutput};
//    vk::SubmitInfo         submit_info {.waitSemaphoreCount   = 1,
//                                        .pWaitSemaphores      = &image_available_semaphore,
//                                        .pWaitDstStageMask    = wait_stages,
//                                        .commandBufferCount   = 1,
//                                        .pCommandBuffers      = &command_buffer,
//                                        .signalSemaphoreCount = 1,
//                                        .pSignalSemaphores    = &render_finished_semaphore};
//
//    graphics_queue.submit(std::array {submit_info}, in_flight_fence);
//
//    vk::PresentInfoKHR present_info {.waitSemaphoreCount = 1,
//                                     .pWaitSemaphores    = &render_finished_semaphore,
//                                     .swapchainCount     = 1,
//                                     .pSwapchains        = &swap_chain,
//                                     .pImageIndices      = &image_index.value,
//                                     .pResults           = nullptr};
//
//    vk::Result present_result;
//    try
//    {
//        present_result = present_queue.presentKHR(present_info);
//    }
//    catch (const vk::OutOfDateKHRError&)
//    {
//        present_result = vk::Result::eErrorOutOfDateKHR;
//    }
//
//    if (present_result == vk::Result::eErrorOutOfDateKHR || present_result == vk::Result::eSuboptimalKHR ||
//        frame_buffer_resized)
//    {
//        frame_buffer_resized = false;
//        RecreateSwapChain();
//    }
//    else if (present_result != vk::Result::eSuccess)
//    {
//        throw std::runtime_error("failed to present swap chain image!");
//    }
//
//    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
//}
//
//
//
//void CreateWindow()
//{
//    glfwInit();
//
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
//
//    window = glfwCreateWindow(800, 600, "Learn Vulkan", nullptr, nullptr);
//
//    glfwSetWindowUserPointer(window, nullptr);
//    glfwSetFramebufferSizeCallback(window,
//                                   [](GLFWwindow* window, int width, int height) { frame_buffer_resized = true; });
//}
//
//void CreateWindowSurface() {
//    glfwCreateWindowSurface(instance, window, nullptr, (VkSurfaceKHR*)&surface);
//}
//
//void InitVulkan()
//{
//    CreateWindow();
//
//    CreateInstance();
//    SetupDebugMessenger();
//
//    CreateWindowSurface();
//
//    SelectPhysicalDevice();
//
//    CreateLogicalDevice();
//
//    CreateSwapChain();
//
//    CreateImageViews();
//
//    CreateRenderPass();
//
//    CreateDescriptorSetLayout();
//
//    CreateGraphicsPipeline();
//
//    CreateCommandPool();
//    CreateCommandBuffer();
//
//    CreateColorResources();
//    CreateDepthResources();
//
//    CreateFrameBuffers();
//
//    CreateSyncObjects();
//
//    CreateTextureImage();
//    CreateTextureImageView();
//    CreateTextureSampler();
//    LoadModel();
//    CreateVertexBuffer();
//    CreateIndexBuffer();
//    CreateUniformBuffers();
//    CreateDescriptorPool();
//    CreateDescriptorSets();
//}
//
//void test_vulkan()
//{
//    InitVulkan();
//
//    // main loop
//    while (!glfwWindowShouldClose(window))
//    {
//        glfwPollEvents();
//        DrawFrame();
//    }
//
//    device.waitIdle();
//
//    cleanup();
//}