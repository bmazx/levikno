#include "lvn_graphics_internal.h"

#include <string.h>

#ifdef LVN_INCLUDE_OPENGL
    #include "lvn_impl_ogl.h"
#endif
#ifdef LVN_INCLUDE_VULKAN
    #include "lvn_impl_vk.h"
#endif


static const char* lvn_getGraphicsApiEnumName(LvnGraphicsApi api);
static LvnResult lvn_setCustomGraphicsContextPfn(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo);

static const char* lvn_getGraphicsApiEnumName(LvnGraphicsApi api)
{
    switch (api)
    {
        case Lvn_GraphicsApi_None:   { return "none"; }
        case Lvn_GraphicsApi_Opengl: { return "opengl"; }
        case Lvn_GraphicsApi_Vulkan: { return "vulkan"; }
    }

    LVN_ASSERT(false, "invalid graphics api enum value");
    return NULL;
}

static LvnResult lvn_setCustomGraphicsContextPfn(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    if (!createInfo->gctxFuncs)
        return Lvn_Result_Failure;

    graphicsctx->implGetSurface = createInfo->gctxFuncs->getSurface;
    graphicsctx->implCreateSurface = createInfo->gctxFuncs->createSurface;
    graphicsctx->implDestroySurface = createInfo->gctxFuncs->destroySurface;
    graphicsctx->implCreateSwapchain = createInfo->gctxFuncs->createSwapchain;
    graphicsctx->implDestroySwapchain = createInfo->gctxFuncs->destroySwapchain;
    graphicsctx->implCreateRenderPass = createInfo->gctxFuncs->createRenderPass;
    graphicsctx->implDestroyRenderPass = createInfo->gctxFuncs->destroyRenderPass;
    graphicsctx->implCreateFramebuffer = createInfo->gctxFuncs->createFramebuffer;
    graphicsctx->implDestroyFramebuffer = createInfo->gctxFuncs->destroyFramebuffer;
    graphicsctx->implCreateShader = createInfo->gctxFuncs->createShader;
    graphicsctx->implDestroyShader = createInfo->gctxFuncs->destroyShader;
    graphicsctx->implCreateDescriptorLayout = createInfo->gctxFuncs->createDescriptorLayout;
    graphicsctx->implDestroyDescriptorLayout = createInfo->gctxFuncs->destroyDescriptorLayout;
    graphicsctx->implCreateDescriptorPool = createInfo->gctxFuncs->createDescriptorPool;
    graphicsctx->implDestroyDescriptorPool = createInfo->gctxFuncs->destroyDescriptorPool;
    graphicsctx->implCreatePipeline = createInfo->gctxFuncs->createPipeline;
    graphicsctx->implDestroyPipeline = createInfo->gctxFuncs->destroyPipeline;
    graphicsctx->implCreateFence = createInfo->gctxFuncs->createFence;
    graphicsctx->implDestroyFence = createInfo->gctxFuncs->destroyFence;
    graphicsctx->implCreateSemaphore = createInfo->gctxFuncs->createSemaphore;
    graphicsctx->implDestroySemaphore = createInfo->gctxFuncs->destroySemaphore;
    graphicsctx->implCreateBuffer = createInfo->gctxFuncs->createBuffer;
    graphicsctx->implDestroyBuffer = createInfo->gctxFuncs->destroyBuffer;
    graphicsctx->implCreateSampler = createInfo->gctxFuncs->createSampler;
    graphicsctx->implDestroySampler = createInfo->gctxFuncs->destroySampler;
    graphicsctx->implCreateTexture = createInfo->gctxFuncs->createTexture;
    graphicsctx->implDestroyTexture = createInfo->gctxFuncs->destroyTexture;
    graphicsctx->implCreateCommandBuffer = createInfo->gctxFuncs->createCommandBuffer;
    graphicsctx->implDestroyCommandBuffer = createInfo->gctxFuncs->destroyCommandBuffer;
    graphicsctx->implAllocateDescriptorSets = createInfo->gctxFuncs->allocateDescriptorSets;
    graphicsctx->implResetDescriptorPool = createInfo->gctxFuncs->resetDescriptorPool;
    graphicsctx->implUpdateDescriptorSets = createInfo->gctxFuncs->updateDescriptorSets;
    graphicsctx->implSurfaceGetSupportedFormats = createInfo->gctxFuncs->surfaceGetSupportedFormats;
    graphicsctx->implSurfaceGetSupportedPresentModes = createInfo->gctxFuncs->surfaceGetSupportedPresentModes;
    graphicsctx->implSwapchainResize = createInfo->gctxFuncs->swapchainResize;
    graphicsctx->implSwapchainAcquireNextImage = createInfo->gctxFuncs->swapchainAcquireNextImage;
    graphicsctx->implFenceWait = createInfo->gctxFuncs->fenceWait;
    graphicsctx->implFenceReset = createInfo->gctxFuncs->fenceReset;
    graphicsctx->implBufferUpdate = createInfo->gctxFuncs->bufferUpdate;
    graphicsctx->implBeginCommandBuffer = createInfo->gctxFuncs->beginCommandBuffer;
    graphicsctx->implEndCommandBuffer = createInfo->gctxFuncs->endCommandBuffer;
    graphicsctx->implCmdBeginRenderPass = createInfo->gctxFuncs->cmdBeginRenderPass;
    graphicsctx->implCmdEndRenderPass = createInfo->gctxFuncs->cmdEndRenderPass;
    graphicsctx->implCmdBindPipeline = createInfo->gctxFuncs->cmdBindPipeline;
    graphicsctx->implCmdBindVertexBuffer = createInfo->gctxFuncs->cmdBindVertexBuffer;
    graphicsctx->implCmdBindIndexBuffer = createInfo->gctxFuncs->cmdBindIndexBuffer;
    graphicsctx->implCmdBindDescriptorSets = createInfo->gctxFuncs->cmdBindDescriptorSets;
    graphicsctx->implCmdSetViewport = createInfo->gctxFuncs->cmdSetViewport;
    graphicsctx->implCmdSetScissor = createInfo->gctxFuncs->cmdSetScissor;
    graphicsctx->implCmdDraw = createInfo->gctxFuncs->cmdDraw;
    graphicsctx->implCmdDrawIndexed = createInfo->gctxFuncs->cmdDrawIndexed;
    graphicsctx->implRenderSubmit = createInfo->gctxFuncs->renderSubmit;
    graphicsctx->implRenderPresent = createInfo->gctxFuncs->renderPresent;

    if (!graphicsctx->implCreateSurface ||
        !graphicsctx->implDestroySurface ||
        !graphicsctx->implCreateSwapchain ||
        !graphicsctx->implDestroySwapchain ||
        !graphicsctx->implCreateRenderPass ||
        !graphicsctx->implDestroyRenderPass ||
        !graphicsctx->implCreateFramebuffer ||
        !graphicsctx->implDestroyFramebuffer ||
        !graphicsctx->implCreateShader ||
        !graphicsctx->implDestroyShader ||
        !graphicsctx->implCreateDescriptorLayout ||
        !graphicsctx->implDestroyDescriptorLayout ||
        !graphicsctx->implCreateDescriptorPool ||
        !graphicsctx->implDestroyDescriptorPool ||
        !graphicsctx->implCreatePipeline ||
        !graphicsctx->implDestroyPipeline ||
        !graphicsctx->implCreateFence ||
        !graphicsctx->implDestroyFence ||
        !graphicsctx->implCreateSemaphore ||
        !graphicsctx->implDestroySemaphore ||
        !graphicsctx->implCreateBuffer ||
        !graphicsctx->implDestroyBuffer ||
        !graphicsctx->implCreateSampler ||
        !graphicsctx->implDestroySampler ||
        !graphicsctx->implCreateTexture ||
        !graphicsctx->implDestroyTexture ||
        !graphicsctx->implCreateCommandBuffer ||
        !graphicsctx->implDestroyCommandBuffer ||
        !graphicsctx->implAllocateDescriptorSets ||
        !graphicsctx->implResetDescriptorPool ||
        !graphicsctx->implUpdateDescriptorSets ||
        !graphicsctx->implSurfaceGetSupportedFormats ||
        !graphicsctx->implSurfaceGetSupportedPresentModes ||
        !graphicsctx->implSwapchainResize ||
        !graphicsctx->implSwapchainAcquireNextImage ||
        !graphicsctx->implFenceWait ||
        !graphicsctx->implFenceReset ||
        !graphicsctx->implBufferUpdate ||
        !graphicsctx->implBeginCommandBuffer ||
        !graphicsctx->implEndCommandBuffer ||
        !graphicsctx->implCmdBeginRenderPass ||
        !graphicsctx->implCmdEndRenderPass ||
        !graphicsctx->implCmdBindPipeline ||
        !graphicsctx->implCmdBindVertexBuffer ||
        !graphicsctx->implCmdBindIndexBuffer ||
        !graphicsctx->implCmdBindDescriptorSets ||
        !graphicsctx->implCmdSetViewport ||
        !graphicsctx->implCmdSetScissor ||
        !graphicsctx->implCmdDraw ||
        !graphicsctx->implCmdDrawIndexed ||
        !graphicsctx->implRenderSubmit ||
        !graphicsctx->implRenderPresent)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to set custom graphics context api functions");
        return Lvn_Result_Failure;
    }

    return Lvn_Result_Success;
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
        LVN_LOG_ERROR(&ctx->coreLogger, "failed to allocate memory for graphics context");
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
    gctxPtr->memory.baseFrameArenaSize = createInfo->memory.baseFrameArenaSize;
    gctxPtr->memory.baseCmdBuffFrameArenaSize = createInfo->memory.baseCmdBuffFrameArenaSize;
    gctxPtr->memory.baseCmdBuffByteStreamSize = createInfo->memory.baseCmdBuffByteStreamSize;

    // setup graphics api
    result = Lvn_Result_Failure;
    switch (createInfo->graphicsapi)
    {
        case Lvn_GraphicsApi_None:
            if (createInfo->gctxFuncs)
                result = lvn_setCustomGraphicsContextPfn(gctxPtr, createInfo);
            break;
        case Lvn_GraphicsApi_Opengl:
#ifdef LVN_INCLUDE_OPENGL
            result = lvnImplOglInit(gctxPtr, createInfo);
#endif
            break;
        case Lvn_GraphicsApi_Vulkan:
#ifdef LVN_INCLUDE_VULKAN
            result = lvnImplVkInit(gctxPtr, createInfo);
#endif
            break;
    }

    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(gctxPtr->coreLogger,
                      "failed to create graphics context, graphics api: %s",
                      lvn_getGraphicsApiEnumName(createInfo->graphicsapi));
        goto fail_cleanup;
    }

    LVN_LOG_TRACE(gctxPtr->coreLogger,
                  "graphics context created: (%p), graphics api: %s",
                  *graphicsctx,
                  lvn_getGraphicsApiEnumName(createInfo->graphicsapi));

    return Lvn_Result_Success;

fail_cleanup:
    if (*graphicsctx)
    {
        lvn_free(*graphicsctx);
        *graphicsctx = NULL;
    }
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
#ifdef LVN_INCLUDE_OPENGL
            lvnImplOglTerminate(graphicsctx);
#endif
            break;
        case Lvn_GraphicsApi_Vulkan:
#ifdef LVN_INCLUDE_VULKAN
            lvnImplVkTerminate(graphicsctx);
#endif
            break;
    }

    LVN_LOG_TRACE(graphicsctx->coreLogger, "graphics context terminated: (%p)", graphicsctx);

    lvn_free(graphicsctx);
}

const LvnSurface* lvnGetSurface(const LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    if (graphicsctx->presentModeFlags & Lvn_PresentationModeFlag_Surface)
        return graphicsctx->implGetSurface(graphicsctx);
    else
        return NULL;
}

LvnResult lvnCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface** surface, const LvnSurfaceCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && surface && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    *surface = NULL;

    *surface = (LvnSurface*) lvn_calloc(sizeof(LvnSurface));
    if (!*surface)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for surface at %p",
                      surface);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnSurface* surfacePtr = *surface;
    surfacePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateSurface(graphicsctx, *surface, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create surface at %p",
                      surface);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*surface)
    {
        lvn_free(*surface);
        *surface = NULL;
    }
    return errResult;
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

    LvnResult errResult = Lvn_Result_Failure;
    *renderpass = NULL;

    *renderpass = (LvnRenderPass*) lvn_calloc(sizeof(LvnRenderPass));
    if (!*renderpass)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for renderpass at %p",
                      renderpass);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnRenderPass* renderpassPtr = *renderpass;
    renderpassPtr->graphicsctx = graphicsctx;

    // create api renderpass
    LvnResult result = graphicsctx->implCreateRenderPass(graphicsctx, *renderpass, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create renderpass at %p",
                      renderpass);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*renderpass)
    {
        lvn_free(*renderpass);
        *renderpass = NULL;
    }
    return errResult;
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

    LvnResult errResult = Lvn_Result_Failure;
    *framebuffer = NULL;

    *framebuffer = (LvnFramebuffer*) lvn_calloc(sizeof(LvnFramebuffer));
    if (!*framebuffer)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for framebuffer at %p",
                      framebuffer);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnFramebuffer* framebufferPtr = *framebuffer;
    framebufferPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateFramebuffer(graphicsctx, *framebuffer, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create framebuffer at %p",
                      framebuffer);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*framebuffer)
    {
        lvn_free(*framebuffer);
        *framebuffer = NULL;
    }
    return errResult;
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

    LvnResult errResult = Lvn_Result_Failure;
    *shader = NULL;

    *shader = (LvnShader*) lvn_calloc(sizeof(LvnShader));
    if (!*shader)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for shader at %p",
                      shader);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnShader* shaderPtr = *shader;
    shaderPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateShader(graphicsctx, *shader, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create shader at %p",
                      shader);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*shader)
    {
        lvn_free(*shader);
        *shader = NULL;
    }
    return errResult;
}

void lvnDestroyShader(LvnShader* shader)
{
    LVN_ASSERT(shader, "shader cannot be null");
    const LvnGraphicsContext* graphicsctx = shader->graphicsctx;
    graphicsctx->implDestroyShader(shader);
    lvn_free(shader);
}

LvnResult lvnCreateDescriptorLayout(const LvnGraphicsContext* graphicsctx, LvnDescriptorLayout** descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && descriptorLayout && createInfo, "graphicsctx, descriptorLayout, and createInfo cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    *descriptorLayout = NULL;

    *descriptorLayout = (LvnDescriptorLayout*) lvn_calloc(sizeof(LvnDescriptorLayout));
    if (!*descriptorLayout)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for descriptorLayout at %p",
                      descriptorLayout);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnDescriptorLayout* descriptorLayoutPtr = *descriptorLayout;
    descriptorLayoutPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateDescriptorLayout(graphicsctx, *descriptorLayout, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create descriptorLayout at %p",
                      descriptorLayout);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*descriptorLayout)
    {
        lvn_free(*descriptorLayout);
        *descriptorLayout = NULL;
    }
    return errResult;
}

void lvnDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
    LVN_ASSERT(descriptorLayout, "descriptorLayout cannot be null");
    const LvnGraphicsContext* graphicsctx = descriptorLayout->graphicsctx;
    graphicsctx->implDestroyDescriptorLayout(descriptorLayout);
    lvn_free(descriptorLayout);
}

LvnResult lvnCreateDescriptorPool(const LvnGraphicsContext* graphicsctx, LvnDescriptorPool** descriptorPool, const LvnDescriptorPoolCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && descriptorPool && createInfo, "graphicsctx, descriptorPool, and createInfo cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    *descriptorPool = NULL;

    *descriptorPool = (LvnDescriptorPool*) lvn_calloc(sizeof(LvnDescriptorPool));
    if (!*descriptorPool)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for descriptorPool at %p",
                      descriptorPool);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnDescriptorPool* descriptorPoolPtr = *descriptorPool;
    descriptorPoolPtr->graphicsctx = graphicsctx;

    // descriptor set memory pool
    LvnMemoryPoolCreateInfo memPoolCreateInfo = {
        .stride = sizeof(LvnDescriptorSet),
        .count = createInfo->maxSets,
        .align = LVN_DEFAULT_ALIGN,
    };

    int cmaResult = lvn_memPoolCreate(&descriptorPoolPtr->setPool, &memPoolCreateInfo);
    if (cmaResult != LVN_CMA_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create set pool for descriptorPool at %p",
                      descriptorPool);
        goto fail_cleanup;
    }

    // create descriptor pool
    LvnResult result = graphicsctx->implCreateDescriptorPool(graphicsctx, *descriptorPool, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create descriptorPool at %p",
                      descriptorPool);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*descriptorPool)
    {
        lvn_memPoolDestroy(&(*descriptorPool)->setPool);
        lvn_free(*descriptorPool);
        *descriptorPool = NULL;
    }
    return errResult;
}

void lvnDestroyDescriptorPool(LvnDescriptorPool* descriptorPool)
{
    LVN_ASSERT(descriptorPool, "descriptorPool cannot be null");
    const LvnGraphicsContext* graphicsctx = descriptorPool->graphicsctx;
    graphicsctx->implDestroyDescriptorPool(descriptorPool);
    lvn_memPoolDestroy(&descriptorPool->setPool);
    lvn_free(descriptorPool);
}

LvnResult lvnCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && pipeline && createInfo, "graphicsctx, pipeline, and createInfo cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    *pipeline = NULL;

    *pipeline = (LvnPipeline*) lvn_calloc(sizeof(LvnPipeline));
    if (!*pipeline)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for pipeline at %p",
                      pipeline);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnPipeline* pipelinePtr = *pipeline;
    pipelinePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreatePipeline(graphicsctx, *pipeline, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create pipeline at %p",
                      pipeline);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*pipeline)
    {
        lvn_free(*pipeline);
        *pipeline = NULL;
    }
    return errResult;
}

void lvnDestroyPipeline(LvnPipeline* pipeline)
{
    LVN_ASSERT(pipeline, "pipeline cannot be null");
    const LvnGraphicsContext* graphicsctx = pipeline->graphicsctx;
    graphicsctx->implDestroyPipeline(pipeline);
    lvn_free(pipeline);
}

LvnResult lvnCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence** fence, bool signaled)
{
    LVN_ASSERT(graphicsctx && fence, "graphicsctx and fence cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    *fence = NULL;

    *fence = (LvnFence*) lvn_calloc(sizeof(LvnFence));
    if (!*fence)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for fence at %p",
                      fence);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnFence* fencePtr = *fence;
    fencePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateFence(graphicsctx, *fence, signaled);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create fence at %p",
                      fence);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*fence)
    {
        lvn_free(*fence);
        *fence = NULL;
    }
    return errResult;
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

    LvnResult errResult = Lvn_Result_Failure;
    *semaphore = NULL;

    *semaphore = (LvnSemaphore*) lvn_calloc(sizeof(LvnSemaphore));
    if (!*semaphore)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for fence at %p",
                      semaphore);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnSemaphore* semaphorePtr = *semaphore;
    semaphorePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateSemaphore(graphicsctx, *semaphore);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create semaphore at %p",
                      semaphore);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*semaphore)
    {
        lvn_free(*semaphore);
        *semaphore = NULL;
    }
    return errResult;
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

    LvnResult errResult = Lvn_Result_Failure;
    *buffer = NULL;

    *buffer = (LvnBuffer*) lvn_calloc(sizeof(LvnBuffer));
    if (!*buffer)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for buffer at %p",
                      buffer);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnBuffer* bufferPtr = *buffer;
    bufferPtr->graphicsctx = graphicsctx;
    bufferPtr->size = createInfo->size;
    bufferPtr->type = createInfo->type;
    bufferPtr->usage = createInfo->usage;

    LvnResult result = graphicsctx->implCreateBuffer(graphicsctx, *buffer, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create buffer at %p",
                      buffer);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*buffer)
    {
        lvn_free(*buffer);
        *buffer = NULL;
    }
    return errResult;
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

    LvnResult errResult = Lvn_Result_Failure;
    *sampler = NULL;

    *sampler = (LvnSampler*) lvn_calloc(sizeof(LvnSampler));
    if (!*sampler)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for sampler at %p",
                      sampler);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnSampler* samplerPtr = *sampler;
    samplerPtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateSampler(graphicsctx, *sampler, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create sampler at %p",
                      sampler);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*sampler)
    {
        lvn_free(*sampler);
        *sampler = NULL;
    }
    return errResult;
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

    // createInfo validation
    if (!createInfo->image)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to create texture at %p | createInfo->image cannot be null", texture);
        return Lvn_Result_Failure;
    }
    if (!createInfo->sampler)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to create texture at %p | createInfo->sampler cannot be null", texture);
        return Lvn_Result_Failure;
    }

    LvnResult errResult = Lvn_Result_Failure;
    *texture = NULL;

    // allocate texture
    *texture = (LvnTexture*) lvn_calloc(sizeof(LvnTexture));
    if (!*texture)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate memory for texture at %p", texture);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnTexture* texturePtr = *texture;
    texturePtr->graphicsctx = graphicsctx;

    LvnResult result = graphicsctx->implCreateTexture(graphicsctx, *texture, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create texture at %p",
                      texture);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*texture)
    {
        lvn_free(*texture);
        *texture = NULL;
    }
    return errResult;
}

void lvnDestroyTexture(LvnTexture* texture)
{
    LVN_ASSERT(texture, "texture cannot be null");
    const LvnGraphicsContext* graphicsctx = texture->graphicsctx;
    graphicsctx->implDestroyTexture(texture);
    lvn_free(texture);
}

LvnResult lvnCreateCommandBuffer(const LvnGraphicsContext* graphicsctx, LvnCommandBuffer** commandBuffer)
{
    LVN_ASSERT(graphicsctx && commandBuffer, "graphicsctx and commandBuffers cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    *commandBuffer = NULL;

    // allocate commandBuffer
    *commandBuffer = (LvnCommandBuffer*) lvn_calloc(sizeof(LvnCommandBuffer));
    if (!*commandBuffer)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to allocate memory for commandBuffer at %p",
                      commandBuffer);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnCommandBuffer* commandBufferPtr = *commandBuffer;
    commandBufferPtr->graphicsctx = graphicsctx;

    // frame arena
    LvnMemoryArenaCreateInfo arenaCreateInfo = {
        .size = graphicsctx->memory.baseCmdBuffFrameArenaSize,
        .align = LVN_DEFAULT_ALIGN,
        .flags = Lvn_MemoryArenaFlag_DynamicGrowth,
    };

    int cmaResult = lvn_memArenaCreate(&commandBufferPtr->frameArena, &arenaCreateInfo);
    if (cmaResult != LVN_CMA_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create memory arena for command buffer at %p",
                      commandBuffer);
        goto fail_cleanup;
    }

    LvnResult result = graphicsctx->implCreateCommandBuffer(graphicsctx, commandBufferPtr);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to create commandBuffer at %p",
                      commandBuffer);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*commandBuffer)
    {
        lvn_memArenaDestroy(&(*commandBuffer)->frameArena);
        lvn_free(*commandBuffer);
        *commandBuffer = NULL;
    }
    return errResult;
}

void lvnDestroyCommandBuffer(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implDestroyCommandBuffer(commandBuffer);
    lvn_memArenaDestroy(&commandBuffer->frameArena);
    lvn_free(commandBuffer);
}

LvnResult lvnAllocateDescriptorSets(const LvnGraphicsContext* graphicsctx, LvnDescriptorSet** pDescriptorSets, LvnDescriptorSetAllocateInfo* allocInfo)
{
    LVN_ASSERT(graphicsctx && pDescriptorSets && allocInfo, "graphicsctx, pDescriptorSets, and allocInfo cannot be null");

    if (!allocInfo->descriptorPool)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "failed to allocate descriptor sets, allocInfo->descriptorPool was null");
        return Lvn_Result_Failure;
    }

    for (uint32_t i = 0; i < allocInfo->descriptorSetCount; i++)
    {
        pDescriptorSets[i] = (LvnDescriptorSet*) lvn_memPoolAlloc(&allocInfo->descriptorPool->setPool);
        if (!pDescriptorSets[i])
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "failed to allocate memory from descriptor set pool for descriptor sets at %p",
                          pDescriptorSets);
            return Lvn_Result_OutOfMemory;
        }
    }

    return graphicsctx->implAllocateDescriptorSets(graphicsctx, pDescriptorSets, allocInfo);
}

LvnResult lvnResetDescriptorPool(const LvnGraphicsContext* graphicsctx, LvnDescriptorPool* descriptorPool)
{
    LVN_ASSERT(graphicsctx && descriptorPool, "graphicsctx and descriptorPool cannot be null");

    LvnResult result = graphicsctx->implResetDescriptorPool(graphicsctx, descriptorPool);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to reset descriptor pool %p",
                      descriptorPool);
        return result;
    }

    result = lvn_memPoolResetMergeBlocks(&descriptorPool->setPool) != Lvn_Result_Success;
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "failed to reset set memory pool for descriptor pool %p",
                      descriptorPool);
        return result;
    }

    return Lvn_Result_Success;
}

LvnResult lvnUpdateDescriptorSets(const LvnGraphicsContext* graphicsctx, uint32_t descriptorWriteCount, const LvnDescriptorSetWriteInfo* pDescriptorWrites, uint32_t descriptorCopyCount, const LvnDescriptorSetCopyInfo* pDescriptorCopies)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");
    return graphicsctx->implUpdateDescriptorSets(graphicsctx, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
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

    // rasterizer
    pipelineFixedFunctions.rasterizer.depthClampEnable = false;
    pipelineFixedFunctions.rasterizer.rasterizerDiscardEnable = false;
    pipelineFixedFunctions.rasterizer.lineWidth = 1.0f;
    pipelineFixedFunctions.rasterizer.cullMode = Lvn_CullFaceMode_Disable;
    pipelineFixedFunctions.rasterizer.frontFace = Lvn_CullFrontFace_Clockwise;
    pipelineFixedFunctions.rasterizer.polygonMode = Lvn_PolygonMode_Fill;
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
    pipelineFixedFunctions.colorBlend.logicOp = Lvn_LogicOp_Copy;
    pipelineFixedFunctions.colorBlend.blendConstants[0] = 0.0f;
    pipelineFixedFunctions.colorBlend.blendConstants[1] = 0.0f;
    pipelineFixedFunctions.colorBlend.blendConstants[2] = 0.0f;
    pipelineFixedFunctions.colorBlend.blendConstants[3] = 0.0f;

    // depth stencil
    pipelineFixedFunctions.depthstencil.depthTestEnable = false;
    pipelineFixedFunctions.depthstencil.depthOpCompare = Lvn_CompareOp_Never;
    pipelineFixedFunctions.depthstencil.stencilTestEnable = false;
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

void lvnBufferUpdate(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset)
{
    LVN_ASSERT(buffer, "buffer cannot be null");
    const LvnGraphicsContext* graphicsctx = buffer->graphicsctx;
    graphicsctx->implBufferUpdate(buffer, data, size, offset);
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

void lvnCmdBindDescriptorSets(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline, uint32_t firstSet, uint32_t descriptorSetCount, LvnDescriptorSet* const* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = commandBuffer->graphicsctx;
    graphicsctx->implCmdBindDescriptorSets(commandBuffer, pipeline, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
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

const char* lvn_getShaderStageEnumName(LvnShaderStageFlagBits stage)
{
    switch (stage)
    {
        case Lvn_ShaderStageFlag_Vertex:   { return "vertex"; }
        case Lvn_ShaderStageFlag_Fragment: { return "fragment"; }
    }

    LVN_ASSERT(false, "invalid stage enum value");
    return NULL;
}
