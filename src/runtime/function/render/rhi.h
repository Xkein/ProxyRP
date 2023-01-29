#pragma once

#include "rhi_resource.h"
#include "rhi_definition.h"
#include "rhi_struct.h"
#include "platform/platform.h"
#include "platform/string.h"


class WindowSystem;
class TextureData;

struct RHIInitInfo
{
    std::shared_ptr<WindowSystem> WindowSystem;
};

class RHI
{
public:
    virtual ~RHI() {};

    virtual void Initialize(RHIInitInfo init_info) = 0;
    virtual void PrepareContext()                  = 0;
    
    // allocate and 
    
    virtual RHIDescriptorSet* AllocateDescriptorSets(const RHIDescriptorSetAllocateInfo* allocate_info) = 0;

    virtual void            CreateSwapChain() = 0;
    virtual void            RecreateSwapChain() = 0;
    virtual void            CreateSwapChainImageViews() = 0;
    virtual RHIFramebuffer* CreateFramebuffer(const RHIFramebufferCreateInfo* pCreateInfo) = 0;
    virtual RHIRenderPass*  CreateRenderPass(const RHIRenderPassCreateInfo* create_info) = 0;
    virtual RHIDescriptorSetLayout*
                               CreateDescriptorSetLayout(const RHIDescriptorSetLayoutCreateInfo* create_info) = 0;
    virtual RHIPipelineLayout* CreatePipelineLayout(const RHIPipelineLayoutCreateInfo* create_info) = 0;
    virtual RHIPipeline*       CreateGraphicsPipeline(RHIPipelineCache*                    pipeline_cache,
                                                      const RHIGraphicsPipelineCreateInfo* create_info) = 0;
    virtual RHIDescriptorPool* CreateDescriptorPool(const RHIDescriptorPoolCreateInfo* create_info) = 0;

    virtual RHISampler* GetOrCreateDefaultSampler(RHIDefaultSamplerType type)     = 0;
    virtual RHISampler* GetOrCreateMipmapSampler(uint32_t width, uint32_t height) = 0;
    virtual RHIShader* CreateShaderModule(const std::vector<byte>& shader_code) = 0;

    virtual void CreateBuffer(RHIDeviceSize          size,
                              RHIBufferUsageFlags    usage,
                              RHIMemoryPropertyFlags properties,
                              RHIBuffer*&             buffer,
                              RHIDeviceMemory*&       buffer_memory) = 0;
    virtual void CreateBufferAndInitialize(RHIDeviceSize          size,
                                           RHIBufferUsageFlags    usage,
                                           RHIMemoryPropertyFlags properties,
                                           RHIBuffer*&             buffer,
                                           RHIDeviceMemory*&       buffer_memory,
                                           void*                   data      = nullptr,
                                           int                     data_size = 0) = 0;
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
                             RHISampleCountFlagBits sample_count = RHISampleCountFlagBits::e1) = 0;
    virtual void CreateImageView(const RHIImage*      image,
                                 RHIFormat           format,
                                 RHIImageAspectFlags image_aspect_flags,
                                 RHIImageViewType    view_type,
                                 uint32_t             layout_count,
                                 uint32_t             miplevels,
                                 RHIImageView*&       image_view) = 0;
    virtual void CreateTextureImage(RHIImage*&         image,
                                    RHIImageView*&     image_view,
                                    RHIDeviceMemory*&  image_memory,
                                    const TextureData* texure_data) = 0;
    virtual void CopyBuffer(RHIBuffer*     src_buffer,
                            RHIBuffer*     dst_buffer,
                            RHIDeviceSize src_offset,
                            RHIDeviceSize dst_offset,
                            RHIDeviceSize size) = 0;

    // command
    virtual RHICommandBuffer* BeginSingleTimeCommands() = 0;
    virtual void              EndSingleTimeCommands(RHICommandBuffer* command_buffer) = 0;

    virtual void UpdateDescriptorSets(const vk::ArrayProxy<const RHIWriteDescriptorSet>& descriptor_writes,
                                      const vk::ArrayProxy<const RHICopyDescriptorSet>&  descriptor_copies) = 0;

    virtual void PushEvent(RHICommandBuffer* commond_buffer, const Char* name, std::array<float, 4> color) = 0;
    virtual void PopEvent(RHICommandBuffer* commond_buffer) = 0;

    // query
    virtual RHIPhysicalDeviceProperties GetPhysicalDeviceProperties() = 0;
    virtual RHICommandBuffer*           GetCommandBuffer() const = 0;
    virtual RHIDescriptorPool*          GetDescriptorPool() const = 0;
    virtual RHISwapchainDesc            GetSwapchainInfo() = 0;
    virtual RHIDepthImageDesc           GetDepthImageInfo() const = 0;
    virtual uint8_t                     GetMaxFramesInFlight() const = 0;
    virtual uint8_t                     GetCurrentFrameIndex() const = 0;

    // destroy
    virtual void Clear() = 0;
    virtual void ClearSwapChain() = 0;
    virtual void DestroyBuffer(RHIBuffer* buffer) = 0;
    virtual void DestroyImage(RHIImage* image) = 0;
    virtual void DestroyImageView(RHIImageView* image_view) = 0;
    virtual void DestroyShaderModule(RHIShader* shader) = 0;

    // memory
    virtual void FreeMemory(RHIDeviceMemory* memory) = 0;
    virtual void* MapMemory(RHIDeviceMemory* memory, vk::DeviceSize offset, vk::DeviceSize size, vk::MemoryMapFlags flags) = 0;
    virtual void UnmapMemory(RHIDeviceMemory* memory) = 0;

    // synchronize

    

    // helpers
    void CreateBuffer(RHIDeviceSize size, RHIBufferUsageFlags usage, RHIMemoryPropertyFlags properties, RHIBufferRef& buffer, RHIDeviceMemoryRef& buffer_memory);
    void CreateBufferAndInitialize(RHIDeviceSize          size,
                                   RHIBufferUsageFlags    usage,
                                   RHIMemoryPropertyFlags properties,
                                   RHIBufferRef&          buffer,
                                   RHIDeviceMemoryRef&    buffer_memory,
                                   void*                  data      = nullptr,
                                   int                    data_size = 0);
    void CopyBuffer(const RHIBufferRef& src_buffer, const RHIBufferRef& dst_buffer, RHIDeviceSize src_offset, RHIDeviceSize dst_offset, RHIDeviceSize size);
    void CreateImage(uint32_t               width,
                     uint32_t               height,
                     RHIFormat              format,
                     RHIImageTiling         tiling,
                     RHIImageUsageFlags     usage,
                     RHIMemoryPropertyFlags properties,
                     RHIImageRef&           image,
                     RHIDeviceMemoryRef&    image_memory,
                     RHIImageCreateFlags    create_flags = {},
                     uint32_t               array_layers = 1,
                     uint32_t               mip_levels   = 1,
                     RHISampleCountFlagBits sample_count = RHISampleCountFlagBits::e1);
    void CreateImageView(const RHIImage*     image,
                         RHIFormat           format,
                         RHIImageAspectFlags image_aspect_flags,
                         RHIImageViewType    view_type,
                         uint32_t            layout_count,
                         uint32_t            miplevels,
                         RHIImageViewRef&    image_view);
    void CreateTextureImage(RHIImageRef& image, RHIImageViewRef& image_view, RHIDeviceMemoryRef& image_memory, const TextureData* texure_data);
};
