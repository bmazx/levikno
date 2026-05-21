#include "lvn_impl_ogl.h"

#if defined(LVN_INCLUDE_X11)
    #include <GL/glx.h>
#endif

#if defined(LVN_INCLUDE_EGL)
    #include "lvn_impl_egl_loader.h"
#endif

#if defined(LVN_PLATFORM_LINUX)
    static const char* s_LvnOglLibName = "libGL.so.1";
#elif defined(LVN_PLATFORM_WINDOWS)
    static const char* s_LvnOglLibName = "opengl32.dll";
#elif defined(LVN_PLATFORM_MACOS)
    static const char* s_LvnOglLibName = "/System/Library/Frameworks/OpenGL.framework/OpenGL";
#endif

static LvnResult lvn_loadOglLoader(LvnOpenglBackends* oglBackends, const LvnGraphicsContextCreateInfo* createInfo);
static void      lvn_unloadOglLoader(LvnOpenglBackends* oglBackends);

static LvnResult lvn_loadOglLoader(LvnOpenglBackends* oglBackends, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(oglBackends && createInfo, "oglBackends and createInfo cannot be null");

    LvnResult result = Lvn_Result_Failure;

#if defined(LVN_INCLUDE_EGL)
    result = lvnEglLoaderInit(oglBackends, createInfo->platformData->ndh, createInfo->platformData->nwh, 1, 1);
#endif

    return result;
}

static void lvn_unloadOglLoader(LvnOpenglBackends* oglBackends)
{
    LVN_ASSERT(oglBackends, "oglBackends cannot be null");

#if defined(LVN_INCLUDE_EGL)
    lvnEglLoaderTerminate(oglBackends);
#endif
}

LvnResult lvnImplOglInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && createInfo, "graphicsctx and createInfo cannot be null");

    LvnOpenglBackends* oglBackends = (LvnOpenglBackends*) lvn_calloc(sizeof(LvnOpenglBackends));
    graphicsctx->implData = oglBackends;

    oglBackends->graphicsctx = graphicsctx;

    // load opengl library
    oglBackends->handle = lvn_platformLoadModule(s_LvnOglLibName);

    if (!oglBackends->handle)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] unable to load opengl shared library: %s",
                      s_LvnOglLibName);
        goto fail_cleanup;
    }

    // load opengl loader
    if (lvn_loadOglLoader(oglBackends, createInfo) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] unable to load opengl loader");
        goto fail_cleanup;
    }

    // opengl loader function symbols
    if (!oglBackends->ogllCreateSurface ||
        !oglBackends->ogllDestroySurface ||
        !oglBackends->ogllMakeCurrent ||
        !oglBackends->ogllSwapBuffers)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to load opengl loader function symbols");
        goto fail_cleanup;
    }

    // opengl function symbols should be loaded in the opengl loader
    if (!oglBackends->glGetString ||
        !oglBackends->glGetError ||
        !oglBackends->glGetIntegerv ||
        !oglBackends->glCreateBuffers ||
        !oglBackends->glDeleteBuffers ||
        !oglBackends->glCreateTextures ||
        !oglBackends->glDeleteTextures ||
        !oglBackends->glCreateFramebuffers ||
        !oglBackends->glDeleteFramebuffers)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to load opengl function symbols");
        goto fail_cleanup;
    }

    oglBackends->glGetIntegerv(GL_MAJOR_VERSION, &oglBackends->versionMajor);
    oglBackends->glGetIntegerv(GL_MINOR_VERSION, &oglBackends->versionMinor);

    graphicsctx->implCreateSurface = lvnImplOglCreateSurface;
    graphicsctx->implDestroySurface = lvnImplOglDestroySurface;
    graphicsctx->implCreateSwapchain = lvnImplOglCreateSwapchain;
    graphicsctx->implDestroySwapchain = lvnImplOglDestroySwapchain;
    graphicsctx->implCreateRenderPass = lvnImplOglCreateRenderPass;
    graphicsctx->implDestroyRenderPass = lvnImplOglDestroyRenderPass;
    graphicsctx->implCreateFramebuffer = lvnImplOglCreateFramebuffer;
    graphicsctx->implDestroyFramebuffer = lvnImplOglDestroyFramebuffer;
    graphicsctx->implCreateShader = lvnImplOglCreateShader;
    graphicsctx->implDestroyShader = lvnImplOglDestroyShader;
    graphicsctx->implCreatePipeline = lvnImplOglCreatePipeline;
    graphicsctx->implDestroyPipeline = lvnImplOglDestroyPipeline;
    graphicsctx->implCreateFence = lvnImplOglCreateFence;
    graphicsctx->implDestroyFence = lvnImplOglDestroyFence;
    graphicsctx->implCreateSemaphore = lvnImplOglCreateSemaphore;
    graphicsctx->implDestroySemaphore = lvnImplOglDestroySemaphore;
    graphicsctx->implCreateBuffer = lvnImplOglsCreateBuffer;
    graphicsctx->implDestroyBuffer = lvnImplOglsDestroyBuffer;
    graphicsctx->implCreateSampler = lvnImplOglsCreateSampler;
    graphicsctx->implDestroySampler = lvnImplOglsDestroySampler;
    graphicsctx->implCreateTexture = lvnImplOglsCreateTexture;
    graphicsctx->implDestroyTexture = lvnImplOglsDestroyTexture;
    graphicsctx->implAllocateCommandBuffers = lvnImplOglAllocateCommandBuffers;
    graphicsctx->implSurfaceGetSupportedFormats = lvnImplOglSurfaceGetSupportedFormats;
    graphicsctx->implSurfaceGetSupportedPresentModes = lvnImplOglSurfaceGetSupportedPresentModes;
    graphicsctx->implSwapchainResize = lvnImplOglSwapchainResize;
    graphicsctx->implSwapchainAcquireNextImage = lvnImplOglSwapchainAcquireNextImage;
    graphicsctx->implFenceWait = lvnImplOglFenceWait;
    graphicsctx->implFenceReset = lvnImplOglFenceReset;
    graphicsctx->implBufferUpdateData = lvnImplOglBufferUpdateData;
    graphicsctx->implBufferResize = lvnImplOglBufferResize;
    graphicsctx->implBeginCommandBuffer = lvnImplOglBeginCommandBuffer;
    graphicsctx->implEndCommandBuffer = lvnImplOglEndCommandBuffer;
    graphicsctx->implCmdBeginRenderPass = lvnImplOglCmdBeginRenderPass;
    graphicsctx->implCmdEndRenderPass = lvnImplOglCmdEndRenderPass;
    graphicsctx->implCmdBindPipeline = lvnImplOglCmdBindPipeline;
    graphicsctx->implCmdBindVertexBuffer = lvnImplOglCmdBindVertexBuffer;
    graphicsctx->implCmdBindIndexBuffer = lvnImplOglCmdBindIndexBuffer;
    graphicsctx->implCmdSetViewport = lvnImplOglCmdSetViewport;
    graphicsctx->implCmdSetScissor = lvnImplOglCmdSetScissor;
    graphicsctx->implCmdDraw = lvnImplOglCmdDraw;
    graphicsctx->implCmdDrawIndexed = lvnImplOglCmdDrawIndexed;
    graphicsctx->implRenderSubmit = lvnImplOglRenderSubmit;
    graphicsctx->implRenderPresent = lvnImplOglRenderPresent;

    return Lvn_Result_Success;

fail_cleanup:
    lvnImplOglTerminate(graphicsctx);
    return Lvn_Result_Failure;
}

void lvnImplOglTerminate(LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    LvnOpenglBackends* oglBackends = (LvnOpenglBackends*) graphicsctx->implData;
    if (!oglBackends)
        return;

    lvn_unloadOglLoader(oglBackends);

    if (oglBackends->handle)
        lvn_platformFreeModule(oglBackends->handle);

    lvn_free(oglBackends);
    graphicsctx->implData = NULL;
}

LvnResult lvnImplOglCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && surface && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    if (oglBackends->ogllCreateSurface(oglBackends, surface, createInfo) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] unable to create api level surface at %p", surface);
        return Lvn_Result_Failure;
    }

    return Lvn_Result_Success;
}

void lvnImplOglDestroySurface(LvnSurface* surface)
{
    LVN_ASSERT(surface, "surface cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) surface->graphicsctx->implData;

    oglBackends->ogllDestroySurface(oglBackends, surface);
}

LvnResult lvnImplOglCreateSwapchain(const LvnGraphicsContext* graphicsctx, LvnSwapchain* swapchain, const LvnSwapchainCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && swapchain && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglSwapchainData* swapchainData = NULL;

    swapchainData = (LvnOglSwapchainData*) lvn_calloc(sizeof(LvnOglSwapchainData));
    if (!swapchainData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for swapchain data in swapchain %p",
                      swapchain);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    swapchainData->images = (GLuint*) lvn_calloc(createInfo->minImageCount * sizeof(GLuint));
    if (!swapchainData->images)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for swapchain images <GLuint> in swapchain %p",
                      swapchain);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    oglBackends->glCreateTextures(GL_TEXTURE_2D, createInfo->minImageCount, swapchainData->images);

    swapchainData->imageCount = createInfo->minImageCount;
    swapchainData->width = createInfo->width;
    swapchainData->height = createInfo->height;
    swapchainData->format = createInfo->surfaceFormat;
    swapchainData->presentMode = createInfo->presentMode;

    swapchain->swapchainData = swapchainData;
    swapchain->swapchainImageCount = createInfo->minImageCount;
    swapchain->extent = (LvnExtent2D){ .width = createInfo->width, .height = createInfo->height };

    return Lvn_Result_Success;

fail_cleanup:
    if (swapchainData)
    {
        if (swapchainData->images)
            lvn_free(swapchainData->images);

        lvn_free(swapchainData);
    }
    return errResult;
}

void lvnImplOglDestroySwapchain(LvnSwapchain* swapchain)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) swapchain->graphicsctx->implData;

    LvnOglSwapchainData* swapchainData = (LvnOglSwapchainData*) swapchain->swapchainData;

    oglBackends->glDeleteTextures(swapchainData->imageCount, swapchainData->images);

    lvn_free(swapchainData->images);
    lvn_free(swapchainData);

    swapchain->swapchainData = NULL;
}

LvnResult lvnImplOglCreateRenderPass(const LvnGraphicsContext* graphicsctx, LvnRenderPass* renderpass, const LvnRenderPassCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && renderpass && createInfo, "graphicsctx, renderpass, and createInfo cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglRenderpassData* renderpassData = NULL;

    renderpassData = (LvnOglRenderpassData*) lvn_calloc(sizeof(LvnOglRenderpassData));
    if (!renderpassData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for renderpass data in renderpass %p",
                      renderpass);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    // color attachments
    renderpassData->colorAttachmentCount = createInfo->colorAttachmentCount;

    renderpassData->colorAttachments = lvn_calloc(createInfo->colorAttachmentCount * sizeof(LvnColorAttachment));
    if (!renderpassData->colorAttachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for color attachments info in renderpass %p",
                      renderpass);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
        renderpassData->colorAttachments[i] = createInfo->pColorAttachments[i];

    // resolve attachments
    renderpassData->resolveAttachments = lvn_calloc(createInfo->colorAttachmentCount * sizeof(LvnResolveAttachment));
    if (!renderpassData->resolveAttachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for resolve attachments info in renderpass %p",
                      renderpass);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    renderpassData->hasResolves = lvn_calloc(createInfo->colorAttachmentCount * sizeof(uint32_t));
    if (!renderpassData->hasResolves)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for hasResolves array in renderpass %p",
                      renderpass);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
    {
        if (createInfo->pColorAttachments[i].resolveAttachment)
        {
            renderpassData->resolveAttachments[i] = *createInfo->pColorAttachments[i].resolveAttachment;
            renderpassData->hasResolves[i] = 1;
        }
    }

    // depth stencil attachment
    if (createInfo->depthStencilAttachment)
    {
        renderpassData->depthStencilAttachment = *createInfo->depthStencilAttachment;
        renderpassData->hasDepth = true;
    }

    renderpass->renderpass = renderpassData;

    return Lvn_Result_Success;

fail_cleanup:
    if (renderpassData)
    {
        if (renderpassData->colorAttachments)
            lvn_free(renderpassData->colorAttachments);
        if (renderpassData->resolveAttachments)
            lvn_free(renderpassData->resolveAttachments);
        if (renderpassData->hasResolves)
            lvn_free(renderpassData->hasResolves);

        lvn_free(renderpassData);
    }
    return errResult;
}

void lvnImplOglDestroyRenderPass(LvnRenderPass* renderpass)
{
    LVN_ASSERT(renderpass, "renderpass cannot be null");

    LvnOglRenderpassData* renderpassData = (LvnOglRenderpassData*) renderpass->renderpass;

    if (renderpassData->colorAttachments)
        lvn_free(renderpassData->colorAttachments);
    if (renderpassData->resolveAttachments)
        lvn_free(renderpassData->resolveAttachments);
    if (renderpassData->hasResolves)
        lvn_free(renderpassData->hasResolves);

    lvn_free(renderpassData);
    renderpass->renderpass = NULL;
}

LvnResult lvnImplOglCreateFramebuffer(const LvnGraphicsContext* graphicsctx, LvnFramebuffer* framebuffer, const LvnFramebufferCreateInfo* createInfo)
{
    return Lvn_Result_Success;
}

void lvnImplOglDestroyFramebuffer(LvnFramebuffer* framebuffer)
{

}

LvnResult lvnImplOglCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    return Lvn_Result_Success;
}

void lvnImplOglDestroyShader(LvnShader* shader)
{

}

LvnResult lvnImplOglCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo)
{
    return Lvn_Result_Success;
}

void lvnImplOglDestroyPipeline(LvnPipeline* pipeline)
{

}

LvnResult lvnImplOglCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence* fence)
{
    return Lvn_Result_Success;
}

void lvnImplOglDestroyFence(LvnFence* fence)
{

}

LvnResult lvnImplOglCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore* semaphore)
{
    return Lvn_Result_Success;
}

void lvnImplOglDestroySemaphore(LvnSemaphore* semaphore)
{

}

LvnResult lvnImplOglsCreateBuffer(const LvnGraphicsContext* graphicsctx, LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo)
{
    return Lvn_Result_Success;
}

void lvnImplOglsDestroyBuffer(LvnBuffer* buffer)
{

}

LvnResult lvnImplOglsCreateSampler(const LvnGraphicsContext* graphicsctx, LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo)
{
    return Lvn_Result_Success;
}

void lvnImplOglsDestroySampler(LvnSampler* sampler)
{

}

LvnResult lvnImplOglsCreateTexture(const LvnGraphicsContext* graphicsctx, LvnTexture* texture, const LvnTextureCreateInfo* createInfo)
{
    return Lvn_Result_Success;
}

void lvnImplOglsDestroyTexture(LvnTexture* texture)
{

}

LvnResult lvnImplOglAllocateCommandBuffers(const LvnGraphicsContext* graphicsctx, const LvnCommandBufferAllocInfo* allocInfo, LvnCommandBuffer** pCommandBuffers)
{
    return Lvn_Result_Success;
}

void lvnImplOglSurfaceGetSupportedFormats(const LvnSurface* surface, uint32_t* formatCount, LvnFormat* pSurfaceFormats)
{
    LVN_ASSERT(surface && formatCount, "surface and formatCount cannot be null");

    *formatCount = 4;

    if (!pSurfaceFormats)
        return;

    pSurfaceFormats[0] = Lvn_Format_R8G8B8A8_UNORM;
    pSurfaceFormats[1] = Lvn_Format_R8G8B8A8_SRGB;
    pSurfaceFormats[2] = Lvn_Format_B8G8R8A8_UNORM;
    pSurfaceFormats[3] = Lvn_Format_B8G8R8A8_SRGB;
}

void lvnImplOglSurfaceGetSupportedPresentModes(const LvnSurface* surface, uint32_t* presentModeCount, LvnPresentMode* pPresentModes)
{
    LVN_ASSERT(surface && presentModeCount, "surface and presentModeCount cannot be null");

    *presentModeCount = 3;

    if (!pPresentModes)
        return;

    pPresentModes[0] = Lvn_PresentMode_FIFO;
    pPresentModes[1] = Lvn_PresentMode_Immediate;
    pPresentModes[2] = Lvn_PresentMode_Mailbox;
}

LvnResult lvnImplOglSwapchainResize(LvnSwapchain* swapchain, uint32_t width, uint32_t height)
{
    return Lvn_Result_Success;
}

LvnResult lvnImplOglSwapchainAcquireNextImage(LvnSwapchain* swapchain, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex)
{
    return Lvn_Result_Success;
}

LvnResult lvnImplOglFenceWait(LvnFence* fence, uint64_t timeout)
{
    return Lvn_Result_Success;
}

LvnResult lvnImplOglFenceReset(LvnFence* fence)
{
    return Lvn_Result_Success;
}

void lvnImplOglBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset)
{

}

void lvnImplOglBufferResize(LvnBuffer* buffer, uint64_t size)
{

}

void lvnImplOglBeginCommandBuffer(LvnCommandBuffer* commandBuffer)
{

}

void lvnImplOglEndCommandBuffer(LvnCommandBuffer* commandBuffer)
{

}

void lvnImplOglCmdBeginRenderPass(LvnCommandBuffer* commandBuffer, LvnRenderPassBeginInfo* beginInfo)
{

}

void lvnImplOglCmdEndRenderPass(LvnCommandBuffer* commandBuffer)
{

}

void lvnImplOglCmdBindPipeline(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline)
{

}

void lvnImplOglCmdBindVertexBuffer(LvnCommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets)
{

}

void lvnImplOglCmdBindIndexBuffer(LvnCommandBuffer* commandBuffer, LvnBuffer* buffer, uint64_t offset)
{

}

void lvnImplOglCmdSetViewport(LvnCommandBuffer* commandBuffer, const LvnViewport* viewport)
{

}

void lvnImplOglCmdSetScissor(LvnCommandBuffer* commandBuffer, const LvnRenderArea* scissor)
{

}

void lvnImplOglCmdDraw(LvnCommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{

}

void lvnImplOglCmdDrawIndexed(LvnCommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{

}

LvnResult lvnImplOglRenderSubmit(const LvnGraphicsContext* graphicsctx, const LvnSubmitInfo* pSubmits, uint32_t submitCount, LvnFence* fence)
{
    return Lvn_Result_Success;
}

LvnResult lvnImplOglRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo)
{
    return Lvn_Result_Success;
}
