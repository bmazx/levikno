#include "lvn_graphics_internal.h"

#include <string.h>

#ifdef LVN_INCLUDE_VULKAN
#include "lvn_impl_vk.h"
#endif

#include "stb_image.h"


static const char* lvn_getGraphicsApiEnumName(LvnGraphicsApi api);

static const char* lvn_getGraphicsApiEnumName(LvnGraphicsApi api)
{
    switch (api)
    {
        case Lvn_GraphicsApi_None:   { return "none"; }
        case Lvn_GraphicsApi_Opengl: { return "opengl"; }
        case Lvn_GraphicsApi_Vulkan: { return "vulkan"; }
    }

    LVN_ASSERT(false, "api in not a valid enum value");
    return NULL;
}

LvnResult lvnCreateGraphicsContext(struct LvnContext* ctx, LvnGraphicsContext** graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(ctx && graphicsctx && createInfo, "ctx, graphicsctx, and createInfo cannot be null");

    if (createInfo->presentationModeFlags & Lvn_PresentationModeFlag_Surface && !createInfo->platformData)
    {
        LVN_LOG_ERROR(&ctx->coreLogger, "failed to create graphics context, createInfo->presentationModeFlags has Lvn_PresentationModeFlag_Surface bit set but createInfo->platformData was null");
        return Lvn_Result_Failure;
    }

    LvnResult result = Lvn_Result_Success;

    // create and init graphics context
    *graphicsctx = (LvnGraphicsContext*) lvn_calloc(sizeof(LvnGraphicsContext));

    if (!*graphicsctx)
    {
        LVN_LOG_ERROR(&ctx->coreLogger, "failed to allocate memory for graphics context at %p", graphicsctx);
        result = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnGraphicsContext* gctxPtr = *graphicsctx;
    gctxPtr->graphicsapi = createInfo->graphicsapi;
    gctxPtr->ctx = ctx;
    gctxPtr->coreLogger = &ctx->coreLogger;
    gctxPtr->presentModeFlags = createInfo->presentationModeFlags;
    gctxPtr->enableGraphicsApiDebugLogging = createInfo->enableGraphicsApiDebugLogging;

    // memory
    // frame arena
    size_t frameArenaSize = (createInfo->memory.baseFrameArenaAllocSize > 0)
        ? createInfo->memory.baseFrameArenaAllocSize
        : 16e+3; // 16 KB
    gctxPtr->frameArena = lvn_memArenaCreate(frameArenaSize, LVN_DEFAULT_ALIGN);

    // cmdBuff pool
    size_t cmdBufPoolSize = (createInfo->memory.baseCmdBuffPoolAllocSize > 0)
        ? createInfo->memory.baseCmdBuffPoolAllocSize
        : 1024;
    gctxPtr->cmdBuffPool = lvn_memPoolCreate(cmdBufPoolSize, sizeof(LvnCommandBuffer), LVN_ALIGNOF(LvnCommandBuffer));


    // setup graphics api
    switch (createInfo->graphicsapi)
    {
        case Lvn_GraphicsApi_None:
            break;
        case Lvn_GraphicsApi_Opengl:
            // TODO: add opengl impl
            break;
        case Lvn_GraphicsApi_Vulkan:
#ifdef LVN_INCLUDE_VULKAN
            result = lvnImplVkInit(gctxPtr, createInfo);
#endif
            break;
    }

    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(gctxPtr->coreLogger, "failed to create graphics context, graphics api: %s",
                      lvn_getGraphicsApiEnumName(createInfo->graphicsapi));
        goto fail_cleanup_setapi;
    }

    LVN_LOG_TRACE(gctxPtr->coreLogger, "graphics context created: (%p), graphics api set: %s",
                  *graphicsctx,
                  lvn_getGraphicsApiEnumName(createInfo->graphicsapi));

    return Lvn_Result_Success;

fail_cleanup_setapi:
    lvn_free(*graphicsctx);
fail_cleanup:
    return result;
}

void lvnDestroyGraphicsContext(LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    switch (graphicsctx->graphicsapi)
    {
        case Lvn_GraphicsApi_None:
            break;
        case Lvn_GraphicsApi_Opengl:
            // TODO: add opengl impl
            break;
        case Lvn_GraphicsApi_Vulkan:
#ifdef LVN_INCLUDE_VULKAN
            lvnImplVkTerminate(graphicsctx);
#endif
            break;
    }

    LVN_LOG_TRACE(graphicsctx->coreLogger, "graphics context terminated: (%p)", graphicsctx);

    lvn_memPoolDestroy(graphicsctx->cmdBuffPool);
    lvn_memArenaDestroy(graphicsctx->frameArena);

    lvn_free(graphicsctx);
}

LvnResult lvnCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface** surface, const LvnSurfaceCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && surface && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    *surface = (LvnSurface*) lvn_calloc(sizeof(LvnSurface));

    if (!*surface)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for surface at %p", surface);
        return Lvn_Result_OutOfMemory;
    }

    LvnSurface* surfacePtr = *surface;
    surfacePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateSurface(graphicsctx, *surface, createInfo);
    if (result != Lvn_Result_Success)
        lvn_free(*surface);

    return result;
}

void lvnDestroySurface(LvnSurface* surface)
{
    LVN_ASSERT(surface, "surface cannot be null");
    const LvnGraphicsContext* graphicsctx = surface->graphicsctx;
    graphicsctx->implDestroySurface(surface);
    lvn_free(surface);
}

LvnResult lvnCreateSwapchain(const LvnGraphicsContext* graphicsctx, LvnSwapchain** swapchain, const LvnSwapchainCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && swapchain && createInfo, "graphicsctx, swapchain, and createInfo cannot be null");

    *swapchain = (LvnSwapchain*) lvn_calloc(sizeof(LvnSwapchain));

    if (!*swapchain)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for swapchain at %p", swapchain);
        return Lvn_Result_OutOfMemory;
    }

    LvnSwapchain* swapchainPtr = *swapchain;
    swapchainPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateSwapchain(graphicsctx, *swapchain, createInfo);
    if (result != Lvn_Result_Success)
        lvn_free(*swapchain);

    return result;
}

void lvnDestroySwapchain(LvnSwapchain* swapchain)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");
    const LvnGraphicsContext* graphicsctx = swapchain->graphicsctx;
    graphicsctx->implDestroySwapchain(swapchain);
    lvn_free(swapchain);
}

LvnResult lvnCreateRenderPass(const LvnGraphicsContext* graphicsctx, LvnRenderPass** renderpass, const LvnRenderPassCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && renderpass && createInfo, "graphicsctx, renderpass, and createInfo cannot be null");

    if (createInfo->colorAttachmentCount > 0 && !createInfo->pColorAttachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create renderpass at (%p), createInfo->colorAttachmentCount is greater than zero (%u) but createInfo->pColorAttachments is null",
                      renderpass, createInfo->colorAttachmentCount);
        return Lvn_Result_Failure;
    }
    if (createInfo->colorAttachmentCount == 0 && !createInfo->depthStencilAttachment)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create renderpass at (%p), createInfo->colorAttachmentCount is zero and createInfo->depthStencilAttachment is null; cannot create renderpass with no attachments",
                      renderpass);
        return Lvn_Result_Failure;
    }

    for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
    {
        if (createInfo->pColorAttachments[i].samples != Lvn_SampleCountFlag_1_Bit && !createInfo->pColorAttachments[i].resolveAttachment)
        {
            LVN_LOG_WARN(graphicsctx->coreLogger,
                          "when creating renderpass at (%p), createInfo->pColorAttachments[%u] has multisampled count but does not have a resolve attachment",
                          renderpass, i);
        }
        if (createInfo->pColorAttachments[i].resolveAttachment)
        {
            if (createInfo->pColorAttachments[i].format != createInfo->pColorAttachments[i].resolveAttachment->format)
            {
                LVN_LOG_ERROR(graphicsctx->coreLogger,
                              "failed to create renderpass at (%p), createInfo->pColorAttachments[%u].format does not have the same format to createInfo->pColorAttachments.resolveAttachment->format; the formats of the color attachment and resolve attachment must be the same",
                              renderpass, i);
                return Lvn_Result_Failure;
            }
        }
        if (createInfo->pColorAttachments[i].samples != createInfo->pColorAttachments[(i + 1) % createInfo->colorAttachmentCount].samples)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "failed to create renderpass at (%p), misaligned sample count, all color attachments must have the same sample count, createInfo->pColorAttachments[%u] and createInfo->pColorAttachments[%u]",
                          renderpass, i, (i + 1) % createInfo->colorAttachmentCount);
            return Lvn_Result_Failure;
        }
        if (createInfo->depthStencilAttachment)
        {
            if (createInfo->pColorAttachments[i].samples != createInfo->depthStencilAttachment->samples)
            {
                LVN_LOG_ERROR(graphicsctx->coreLogger,
                              "failed to create renderpass at (%p), createInfo->pColorAttachments[%u].samples does not have the same sample count to createInfo->depthStencilAttachment->samples; the depthStencilAttachment must have the same sample count to the color attachments",
                              renderpass, i);
                return Lvn_Result_Failure;
            }
        }
    }

    *renderpass = (LvnRenderPass*) lvn_calloc(sizeof(LvnRenderPass));

    if (!*renderpass)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for renderpass at %p", renderpass);
        return Lvn_Result_OutOfMemory;
    }

    LvnRenderPass* renderpassPtr = *renderpass;
    renderpassPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateRenderPass(graphicsctx, *renderpass, createInfo);
    if (result != Lvn_Result_Success)
        lvn_free(*renderpass);

    return result;
}

void lvnDestroyRenderPass(LvnRenderPass* renderpass)
{
    LVN_ASSERT(renderpass, "renderpass cannot be null");
    const LvnGraphicsContext* graphicsctx = renderpass->graphicsctx;
    graphicsctx->implDestroyRenderPass(renderpass);
    lvn_free(renderpass);
}

LvnResult lvnCreateFramebuffer(const LvnGraphicsContext* graphicsctx, LvnFramebuffer** framebuffer, const LvnFramebufferCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && framebuffer && createInfo, "graphicsctx, framebuffer, and createInfo cannot be null");

    *framebuffer = (LvnFramebuffer*) lvn_calloc(sizeof(LvnFramebuffer));

    if (!*framebuffer)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for framebuffer at %p", framebuffer);
        return Lvn_Result_OutOfMemory;
    }

    LvnFramebuffer* framebufferPtr = *framebuffer;
    framebufferPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateFramebuffer(graphicsctx, *framebuffer, createInfo);
    if (result != Lvn_Result_Success)
        lvn_free(*framebuffer);

    return result;
}

void lvnDestroyFramebuffer(LvnFramebuffer* framebuffer)
{
    LVN_ASSERT(framebuffer, "framebuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = framebuffer->graphicsctx;
    graphicsctx->implDestroyFramebuffer(framebuffer);
    lvn_free(framebuffer);
}

LvnResult lvnCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader** shader, const LvnShaderCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && shader && createInfo, "graphicsctx, shader, and createInfo cannot be null");

    *shader = (LvnShader*) lvn_calloc(sizeof(LvnShader));

    if (!*shader)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for shader at %p", shader);
        return Lvn_Result_OutOfMemory;
    }

    LvnShader* shaderPtr = *shader;
    shaderPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateShader(graphicsctx, *shader, createInfo);
    if (result != Lvn_Result_Success)
        lvn_free(*shader);

    return result;
}

void lvnDestroyShader(LvnShader* shader)
{
    LVN_ASSERT(shader, "shader cannot be null");
    const LvnGraphicsContext* graphicsctx = shader->graphicsctx;
    graphicsctx->implDestroyShader(shader);
    lvn_free(shader);
}

LvnResult lvnCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && pipeline && createInfo, "graphicsctx, pipeline, and createInfo cannot be null");

    *pipeline = (LvnPipeline*) lvn_calloc(sizeof(LvnPipeline));

    if (!*pipeline)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for pipeline at %p", pipeline);
        return Lvn_Result_OutOfMemory;
    }

    LvnPipeline* pipelinePtr = *pipeline;
    pipelinePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreatePipeline(graphicsctx, *pipeline, createInfo);
    if (result != Lvn_Result_Success)
        lvn_free(*pipeline);

    return result;
}

void lvnDestroyPipeline(LvnPipeline* pipeline)
{
    LVN_ASSERT(pipeline, "pipeline cannot be null");
    const LvnGraphicsContext* graphicsctx = pipeline->graphicsctx;
    graphicsctx->implDestroyPipeline(pipeline);
    lvn_free(pipeline);
}

LvnResult lvnCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence** fence)
{
    LVN_ASSERT(graphicsctx && fence, "graphicsctx and fence cannot be null");

    *fence = (LvnFence*) lvn_calloc(sizeof(LvnFence));

    if (!*fence)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for fence at %p", fence);
        return Lvn_Result_OutOfMemory;
    }

    LvnFence* fencePtr = *fence;
    fencePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateFence(graphicsctx, *fence);
    if (result != Lvn_Result_Success)
        lvn_free(*fence);

    return result;
}

void lvnDestroyFence(LvnFence* fence)
{
    LVN_ASSERT(fence, "fence cannot be null");
    const LvnGraphicsContext* graphicsctx = fence->graphicsctx;
    graphicsctx->implDestroyFence(fence);
    lvn_free(fence);
}

LvnResult lvnCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore** semaphore)
{
    LVN_ASSERT(graphicsctx && semaphore, "graphicsctx and semaphore cannot be null");

    *semaphore = (LvnSemaphore*) lvn_calloc(sizeof(LvnSemaphore));

    if (!*semaphore)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for fence at %p", semaphore);
        return Lvn_Result_OutOfMemory;
    }

    LvnSemaphore* semaphorePtr = *semaphore;
    semaphorePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateSemaphore(graphicsctx, *semaphore);
    if (result != Lvn_Result_Success)
        lvn_free(*semaphore);

    return result;
}

void lvnDestroySemaphore(LvnSemaphore* semaphore)
{
    LVN_ASSERT(semaphore, "semaphore cannot be null");
    const LvnGraphicsContext* graphicsctx = semaphore->graphicsctx;
    graphicsctx->implDestroySemaphore(semaphore);
    lvn_free(semaphore);
}

LvnResult lvnCreateBuffer(const LvnGraphicsContext* graphicsctx, LvnBuffer** buffer, const LvnBufferCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && buffer && createInfo, "graphicsctx, buffer, and createInfo cannot be null");

    *buffer = (LvnBuffer*) lvn_calloc(sizeof(LvnBuffer));

    if (!*buffer)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for buffer at %p", buffer);
        return Lvn_Result_OutOfMemory;
    }

    LvnBuffer* bufferPtr = *buffer;
    bufferPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateBuffer(graphicsctx, *buffer, createInfo);
    if (result != Lvn_Result_Success)
        lvn_free(*buffer);

    return result;
}

void lvnDestroyBuffer(LvnBuffer* buffer)
{
    LVN_ASSERT(buffer, "buffer cannot be null");
    const LvnGraphicsContext* graphicsctx = buffer->graphicsctx;
    graphicsctx->implDestroyBuffer(buffer);
    lvn_free(buffer);
}

LvnResult lvnCreateSampler(const LvnGraphicsContext* graphicsctx, LvnSampler** sampler, const LvnSamplerCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && sampler && createInfo, "graphicsctx, sampler, and createInfo cannot be null");

    *sampler = (LvnSampler*) lvn_calloc(sizeof(LvnSampler));

    if (!*sampler)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for sampler at %p", sampler);
        return Lvn_Result_OutOfMemory;
    }

    LvnSampler* samplerPtr = *sampler;
    samplerPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateSampler(graphicsctx, *sampler, createInfo);
    if (result != Lvn_Result_Success)
        lvn_free(*sampler);

    return result;
}

void lvnDestroySampler(LvnSampler* sampler)
{
    LVN_ASSERT(sampler, "sampler cannot be null");
    const LvnGraphicsContext* graphicsctx = sampler->graphicsctx;
    graphicsctx->implDestroySampler(sampler);
    lvn_free(sampler);
}

LvnResult lvnCreateTexture(const LvnGraphicsContext* graphicsctx, LvnTexture** texture, const LvnTextureCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && texture && createInfo, "graphicsctx, texture, and createInfo cannot be null");

    *texture = (LvnTexture*) lvn_calloc(sizeof(LvnTexture));

    if (!*texture)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for sampler at %p", texture);
        return Lvn_Result_OutOfMemory;
    }

    LvnTexture* texturePtr = *texture;
    texturePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateTexture(graphicsctx, *texture, createInfo);
    if (result != Lvn_Result_Success)
        lvn_free(*texture);

    return result;
}

void lvnDestroyTexture(LvnTexture* texture)
{
    LVN_ASSERT(texture, "texture cannot be null");
    const LvnGraphicsContext* graphicsctx = texture->graphicsctx;
    graphicsctx->implDestroyTexture(texture);
    lvn_free(texture);
}

LvnResult lvnAllocateCommandBuffers(const LvnGraphicsContext* graphicsctx, const LvnCommandBufferAllocInfo* allocInfo, LvnCommandBuffer** pCommandBuffers)
{
    LVN_ASSERT(graphicsctx && allocInfo && pCommandBuffers, "graphicsctx, allocInfo, and pCommandBuffers cannot be null");

    for (uint32_t i = 0; i < allocInfo->count; i++)
    {
        pCommandBuffers[i] = (LvnCommandBuffer*) lvn_memPoolAlloc(graphicsctx->cmdBuffPool);
        if (!pCommandBuffers[i])
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate pCommandBuffers[%u] of array size %u",
                          i, allocInfo->count);
            goto fail_cleanup;
        }

        memset(pCommandBuffers[i], 0, sizeof(LvnCommandBuffer));
        pCommandBuffers[i]->graphicsctx = graphicsctx;
    }

    LvnResult result = graphicsctx->implAllocateCommandBuffers(graphicsctx, allocInfo, pCommandBuffers);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate graphics side command buffers");
        goto fail_cleanup;
    }

    return result;

fail_cleanup:
    for (uint32_t i = 0; i < allocInfo->count; i++)
        lvn_memPoolFree(graphicsctx->cmdBuffPool, pCommandBuffers[i]);

    return Lvn_Result_Failure;
}

void lvnSurfaceGetSupportedFormats(const LvnSurface* surface, uint32_t* formatCount, LvnFormat* pSurfaceFormats)
{
    LVN_ASSERT(surface && formatCount, "surface and formatCount cannot be null");
    const LvnGraphicsContext* graphicsctx = surface->graphicsctx;
    graphicsctx->implSurfaceGetSupportedFormats(surface, formatCount, pSurfaceFormats);
}

void lvnSurfaceGetSupportedPresentModes(const LvnSurface* surface, uint32_t* presentModeCount, LvnPresentMode* pPresentModes)
{
    LVN_ASSERT(surface && presentModeCount, "surface and presentModeCount cannot be null");
    const LvnGraphicsContext* graphicsctx = surface->graphicsctx;
    graphicsctx->implSurfaceGetSupportedPresentModes(surface, presentModeCount, pPresentModes);
}

LvnFormat lvnSwapchainGetFormat(const LvnSwapchain* swapchain)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");
    return swapchain->swapchainColorFormat;
}

LvnTexture* lvnSwapchainGetImage(LvnSwapchain* swapchain, uint32_t imageIndex)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");
    LVN_ASSERT(imageIndex < swapchain->swapchainImageCount, "imageIndex out of index bounds");
    return &swapchain->pSwapchainImages[imageIndex];
}

uint32_t lvnSwapchainGetImageCount(const LvnSwapchain* swapchain)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");
    return swapchain->swapchainImageCount;
}

LvnExtent2D lvnSwapchainGetExtent(const LvnSwapchain* swapchain)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");
    return swapchain->extent;
}

LvnResult lvnSwapchainResize(LvnSwapchain* swapchain, uint32_t width, uint32_t height)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");
    const LvnGraphicsContext* graphicsctx = swapchain->graphicsctx;
    return graphicsctx->implSwapchainResize(swapchain, width, height);
}

LvnResult lvnSwapchainAcquireNextImage(LvnSwapchain* swapchain, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex)
{
    LVN_ASSERT(swapchain && imageIndex, "swapchain and imageIndex cannot be null");
    const LvnGraphicsContext* graphicsctx = swapchain->graphicsctx;
    return graphicsctx->implSwapchainAcquireNextImage(swapchain, semaphore, fence, imageIndex);
}

LvnPipelineFixedFunctions lvnConfigPipelineFixedFunctionsInit(void)
{
    LvnPipelineFixedFunctions pipelineFixedFunctions = {0};

    // input assembly
    pipelineFixedFunctions.inputAssembly.topology = Lvn_TopologyType_Triangle;
    pipelineFixedFunctions.inputAssembly.primitiveRestartEnable = false;

    // viewport
    pipelineFixedFunctions.viewport.x = 0.0f;
    pipelineFixedFunctions.viewport.y = 0.0f;
    pipelineFixedFunctions.viewport.width = 0.0f;
    pipelineFixedFunctions.viewport.height = 0.0f;
    pipelineFixedFunctions.viewport.minDepth = 0.0f;
    pipelineFixedFunctions.viewport.maxDepth = 1.0f;

    // scissor
    pipelineFixedFunctions.scissor.offset.x = 0;
    pipelineFixedFunctions.scissor.offset.y = 0;
    pipelineFixedFunctions.scissor.extent.width = 0;
    pipelineFixedFunctions.scissor.extent.height = 0;

    // rasterizer
    pipelineFixedFunctions.rasterizer.depthClampEnable = false;
    pipelineFixedFunctions.rasterizer.rasterizerDiscardEnable = false;
    pipelineFixedFunctions.rasterizer.lineWidth = 1.0f;
    pipelineFixedFunctions.rasterizer.cullMode = Lvn_CullFaceMode_Disable;
    pipelineFixedFunctions.rasterizer.frontFace = Lvn_CullFrontFace_Clockwise;
    pipelineFixedFunctions.rasterizer.depthBiasEnable = false;
    pipelineFixedFunctions.rasterizer.depthBiasConstantFactor = 0.0f;
    pipelineFixedFunctions.rasterizer.depthBiasClamp = 0.0f;
    pipelineFixedFunctions.rasterizer.depthBiasSlopeFactor = 0.0f;

    // multisampling
    pipelineFixedFunctions.multisampling.sampleShadingEnable = false;
    pipelineFixedFunctions.multisampling.rasterizationSamples = Lvn_SampleCountFlag_1_Bit;
    pipelineFixedFunctions.multisampling.minSampleShading = 1.0f;
    pipelineFixedFunctions.multisampling.sampleMask = NULL;
    pipelineFixedFunctions.multisampling.alphaToCoverageEnable = false;
    pipelineFixedFunctions.multisampling.alphaToOneEnable = false;

    // color attachments
    pipelineFixedFunctions.colorBlend.colorBlendAttachmentCount = 0; // if no attachments are provided, an attachment will automatically be created
    pipelineFixedFunctions.colorBlend.pColorBlendAttachments = NULL;

    // color blend
    pipelineFixedFunctions.colorBlend.logicOpEnable = false;
    pipelineFixedFunctions.colorBlend.blendConstants[0] = 0.0f;
    pipelineFixedFunctions.colorBlend.blendConstants[1] = 0.0f;
    pipelineFixedFunctions.colorBlend.blendConstants[2] = 0.0f;
    pipelineFixedFunctions.colorBlend.blendConstants[3] = 0.0f;

    // depth stencil
    pipelineFixedFunctions.depthstencil.enableDepth = false;
    pipelineFixedFunctions.depthstencil.depthOpCompare = Lvn_CompareOp_Never;
    pipelineFixedFunctions.depthstencil.enableStencil = false;
    pipelineFixedFunctions.depthstencil.stencil.compareMask = 0;
    pipelineFixedFunctions.depthstencil.stencil.writeMask = 0;
    pipelineFixedFunctions.depthstencil.stencil.reference = 0;
    pipelineFixedFunctions.depthstencil.stencil.compareOp = Lvn_CompareOp_Never;
    pipelineFixedFunctions.depthstencil.stencil.depthFailOp = Lvn_StencilOp_Keep;
    pipelineFixedFunctions.depthstencil.stencil.failOp = Lvn_StencilOp_Keep;
    pipelineFixedFunctions.depthstencil.stencil.passOp = Lvn_StencilOp_Keep;

    return pipelineFixedFunctions;
}

LvnResult lvnFenceWait(LvnFence* fence, uint64_t timeout)
{
    LVN_ASSERT(fence, "fence cannot be null");
    const LvnGraphicsContext* graphicsctx = fence->graphicsctx;
    return graphicsctx->implFenceWait(fence, timeout);
}

LvnResult lvnFenceReset(LvnFence* fence)
{
    LVN_ASSERT(fence, "fence cannot be null");
    const LvnGraphicsContext* graphicsctx = fence->graphicsctx;
    return graphicsctx->implFenceReset(fence);
}

void lvnBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset)
{
    LVN_ASSERT(buffer, "buffer cannot be null");
    const LvnGraphicsContext* graphicsctx = buffer->graphicsctx;

    if (buffer->usage == Lvn_BufferUsage_Static)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "cannot update data buffer (%p) that has static buffer usage Lvn_BufferUsage_Static",
                      buffer);
        return;
    }

    graphicsctx->implBufferUpdateData(buffer, data, size, offset);
}

void lvnBufferResize(LvnBuffer* buffer, uint64_t size)
{
    LVN_ASSERT(buffer, "buffer cannot be null");
    const LvnGraphicsContext* graphicsctx = buffer->graphicsctx;

    if (buffer->usage != Lvn_BufferUsage_Resize)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "cannot resize buffer (%p) that does not have resize buffer usage Lvn_BufferUsage_Resize",
                      buffer);
        return;
    }

    graphicsctx->implBufferResize(buffer, size);
}

LvnImage lvnLoadImage(const char* filepath, int forceChannels, bool flipVertically)
{
    LVN_ASSERT(filepath, "filepath cannot not be null");
    LVN_ASSERT(forceChannels >= 0 && forceChannels <= 4, "forceChannels must be between 0 and 4");

    LvnImage image = {0};

    stbi_set_flip_vertically_on_load(flipVertically);
    int imageWidth, imageHeight, imageChannels;
    stbi_uc* pixels = stbi_load(filepath, &imageWidth, &imageHeight, &imageChannels, forceChannels);
    if (!pixels)
        return image;

    void* data = lvn_calloc(imageWidth * imageHeight * (forceChannels ? forceChannels : imageChannels));
    if (!data) {
        stbi_image_free(pixels);
        return image;
    }

    image.width = imageWidth;
    image.height = imageHeight;
    image.channels = forceChannels ? forceChannels : imageChannels;
    image.data = data;

    stbi_image_free(pixels);

    return image;
}

void lvnUnloadImage(LvnImage* image)
{
    if (!image) return;
    lvn_free(image->data);
    image->data = NULL;
    image->width = 0;
    image->height = 0;
    image->channels = 0;
}

void lvnBeginCommandBuffer(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implBeginCommandBuffer(commandBuffer);
}

void lvnEndCommandBuffer(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implEndCommandBuffer(commandBuffer);
}

void lvnCmdBeginRenderPass(LvnCommandBuffer* commandBuffer, LvnRenderPassBeginInfo* beginInfo)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdBeginRenderPass(commandBuffer, beginInfo);
}

void lvnCmdEndRenderPass(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdEndRenderPass(commandBuffer);
}

void lvnCmdBindPipeline(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdBindPipeline(commandBuffer, pipeline);
}

void lvnCmdBindVertexBuffer(LvnCommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdBindVertexBuffer(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

void lvnCmdBindIndexBuffer(LvnCommandBuffer* commandBuffer, LvnBuffer* buffer, uint64_t offset)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdBindIndexBuffer(commandBuffer, buffer, offset);
}

void lvnCmdSetViewport(LvnCommandBuffer* commandBuffer, const LvnViewport* viewport)
{
    LVN_ASSERT(commandBuffer && viewport, "commandBuffer and viewport cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdSetViewport(commandBuffer, viewport);
}

void lvnCmdSetScissor(LvnCommandBuffer* commandBuffer, const LvnRenderArea* scissor)
{
    LVN_ASSERT(commandBuffer && scissor, "commandBuffer and scissor cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdSetScissor(commandBuffer, scissor);
}

void lvnCmdDraw(LvnCommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void lvnCmdDrawIndexed(LvnCommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

LvnResult lvnRenderSubmit(const LvnGraphicsContext* graphicsctx, const LvnSubmitInfo* pSubmits, uint32_t submitCount, LvnFence* fence)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");
    return graphicsctx->implRenderSubmit(graphicsctx, pSubmits, submitCount, fence);
}

LvnResult lvnRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo)
{
    LVN_ASSERT(graphicsctx && presentInfo, "graphicsctx and presentInfo cannot be null");
    return graphicsctx->implRenderPresent(graphicsctx, presentInfo);
}
