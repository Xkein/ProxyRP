#pragma once

#include <memory>

class RHIResource
{

};

class RHIShader : RHIResource
{

};

class RHITexture : RHIResource
{

};


class RHIUniformBuffer : RHIResource
{

};

class RHIIndexBuffer : RHIResource
{

};

class RHIVertexBuffer : RHIResource
{

};

typedef std::shared_ptr<RHIUniformBuffer> RHIUniformBufferRef;
typedef std::shared_ptr<RHIIndexBuffer>   RHIIndexBufferRef;
typedef std::shared_ptr<RHIVertexBuffer>  RHIVertexBufferRef;


typedef std::shared_ptr<RHITexture> RHITextureRef;

typedef std::shared_ptr<RHIShader> RHIShaderRef;
