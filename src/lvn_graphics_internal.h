#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "lvn_graphics.h"
#include "levikno_internal.h"


struct LvnImageView
{
    void*      imageHandle;
    void*      imageViewHandle;
    int32_t    imageLayoutEnum;
    int32_t    formatEnum;
};

struct LvnFramebuffer
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        framebufferData;
};

struct LvnSurface
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        surface;
    void*                        swapchainData;
    LvnImageView*                pSwapchainImageViews;
    uint32_t                     swapchainImageViewCount;
    LvnFormat                    swapchainColorFormat;
    LvnExtent2D                  extent;
};

struct LvnDescriptorLayout
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        descriptorLayout;
};

struct LvnShader
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        shader;
};

struct LvnPipeline
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        pipelineHandle;
    void*                        pipelineLayoutHandle;
};

struct LvnCommandBuffer
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        commandbuffer;
    LvnImageView**               pColorAttachmentImages;       // use in vulkan to store swapchain color attachment images per rendering/renderpass
    uint32_t                     colorAttachmentImageCount;    // vulkan color attachment image count
};

struct LvnFence
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        fenceHandle;
};

struct LvnSemaphore
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        semaphoreHandle;
};

struct LvnBuffer
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        buffer;
    void*                        bufferMemory;
    void*                        bufferMap;
    uint64_t                     size;
    LvnBufferTypeFlagBits        type;
    LvnBufferUsage               usage;
    uint32_t                     id;
};

struct LvnGraphicsContext
{
    LvnGraphicsApi              graphicsapi;
    const LvnContext*           ctx;
    const LvnLogger*            coreLogger;
    LvnPresentationModeFlags    presentModeFlags;
    bool                        enableGraphicsApiDebugLogging;

    LvnMemoryPool*              cmdBuffPool;
    LvnMemoryArena*             frameArena;

    // graphics implementation
    void*                       implData;
    LvnResult                   (*implCreateSurface)(const LvnGraphicsContext*, LvnSurface*, const LvnSurfaceCreateInfo*);
    void                        (*implDestroySurface)(LvnSurface*);
    LvnResult                   (*implCreateShader)(const LvnGraphicsContext*, LvnShader*, const LvnShaderCreateInfo*);
    void                        (*implDestroyShader)(LvnShader*);
    LvnResult                   (*implCreatePipeline)(const LvnGraphicsContext*, LvnPipeline*, const LvnPipelineCreateInfo*);
    void                        (*implDestroyPipeline)(LvnPipeline*);
    LvnResult                   (*implCreateFence)(const LvnGraphicsContext*, LvnFence*);
    void                        (*implDestroyFence)(LvnFence*);
    LvnResult                   (*implCreateSemaphore)(const LvnGraphicsContext*, LvnSemaphore*);
    void                        (*implDestroySemaphore)(LvnSemaphore*);
    LvnResult                   (*implCreateBuffer)(const LvnGraphicsContext*, LvnBuffer*, const LvnBufferCreateInfo*);
    void                        (*implDestroyBuffer)(LvnBuffer*);
    LvnResult                   (*implAllocateCommandBuffers)(const LvnGraphicsContext*, const LvnCommandBufferAllocInfo*, LvnCommandBuffer**);

    LvnResult                   (*implSurfaceResize)(LvnSurface*, uint32_t, uint32_t);
    LvnResult                   (*implFenceWait)(LvnFence*, uint64_t);
    LvnResult                   (*implFenceReset)(LvnFence*);

    void                        (*implBufferUpdateData)(LvnBuffer*, void*, uint64_t, uint64_t);
    void                        (*implBufferResize)(LvnBuffer*, uint64_t);


    void                        (*implBeginCommandBuffer)(LvnCommandBuffer*);
    void                        (*implEndCommandBuffer)(LvnCommandBuffer*);
    void                        (*implCmdBeginRendering)(LvnCommandBuffer*, const LvnRenderingInfo*);
    void                        (*implCmdEndRendering)(LvnCommandBuffer*);
    void                        (*implCmdBindPipeline)(LvnCommandBuffer*, LvnPipeline*);
    void                        (*implCmdBindVertexBuffer)(LvnCommandBuffer*, uint32_t, uint32_t, LvnBuffer**, uint64_t*);
    void                        (*implCmdBindIndexBuffer)(LvnCommandBuffer*, LvnBuffer*, uint64_t);
    void                        (*implCmdSetViewport)(LvnCommandBuffer*, const LvnViewport*);
    void                        (*implCmdSetScissor)(LvnCommandBuffer*, const LvnRenderArea*);
    void                        (*implCmdDraw)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t);
    void                        (*implCmdDrawIndexed)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
    LvnResult                   (*implSurfaceAcquireNextImage)(LvnSurface*, LvnSemaphore*, LvnFence*, uint32_t*);
    LvnResult                   (*implRenderSubmit)(const LvnGraphicsContext*, const LvnSubmitInfo*, uint32_t, LvnFence*);
    LvnResult                   (*implRenderPresent)(const LvnGraphicsContext*, const LvnPresentInfo*);
};


#endif // !HG_LVN_GRAPHICS_INTERNAL_H
