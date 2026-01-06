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
    const LvnGraphicsContext* graphicsctx;
    void* framebufferData;
};

struct LvnSurface
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        surface;
    void*                        swapchainData;
    LvnImageView*                pSwapchainImageViews;
    uint32_t                     swapchainImageViewCount;
    LvnFormat                    swapchainColorFormat;
};

struct LvnDescriptorLayout
{
    const LvnGraphicsContext* graphicsctx;
    void* descriptorLayout;
};

struct LvnShader
{
    const LvnGraphicsContext* graphicsctx;
    void* shader;
};

struct LvnPipeline
{
    const LvnGraphicsContext* graphicsctx;
    void* pipelineHandle;
    void* pipelineLayoutHandle;
};

struct LvnCommandBuffer
{
    const LvnGraphicsContext*       graphicsctx;
    void*                           commandbuffer;
    LvnImageView**                  pColorAttachmentImages;       // use in vulkan to store swapchain color attachment images per rendering/renderpass
    uint32_t                        colorAttachmentImageCount;    // vulkan color attachment image count
};

struct LvnFence
{
    const LvnGraphicsContext* graphicsctx;
    void* fenceHandle;
};

struct LvnSemaphore
{
    const LvnGraphicsContext* graphicsctx;
    void* semaphoreHandle;
};

struct LvnGraphicsContext
{
    LvnGraphicsApi              graphicsapi;
    const LvnContext*           ctx;
    LvnLogger*                  coreLogger;
    LvnPresentationModeFlags    presentModeFlags;
    bool                        enableGraphicsApiDebugLogging;

    // graphics implementation
    void*                       implData;
    LvnResult                   (*implCreateSurface)(const LvnGraphicsContext*, LvnSurface*, const LvnSurfaceCreateInfo*);
    void                        (*implDestroySurface)(LvnSurface*);
    LvnResult                   (*implCreateShader)(const LvnGraphicsContext*, LvnShader*, const LvnShaderCreateInfo*);
    void                        (*implDestroyShader)(LvnShader*);
    LvnResult                   (*implCreatePipeline)(const LvnGraphicsContext*, LvnPipeline*, const LvnPipelineCreateInfo*);
    void                        (*implDestroyPipeline)(LvnPipeline*);
    LvnResult                   (*implCreateCommandBuffer)(const LvnGraphicsContext*, LvnCommandBuffer*, const LvnCommandBufferCreateInfo*);
    void                        (*implDestroyCommandBuffer)(LvnCommandBuffer*);
    LvnResult                   (*implCreateFence)(const LvnGraphicsContext*, LvnFence*);
    void                        (*implDestroyFence)(LvnFence*);
    LvnResult                   (*implCreateSemaphore)(const LvnGraphicsContext*, LvnSemaphore*);
    void                        (*implDestroySemaphore)(LvnSemaphore*);

    LvnResult                   (*implFenceWait)(LvnFence*, uint64_t);
    LvnResult                   (*implFenceReset)(LvnFence*);

    void                        (*implBeginCommandBuffer)(LvnCommandBuffer*);
    void                        (*implEndCommandBuffer)(LvnCommandBuffer*);
    void                        (*implCmdBeginRendering)(LvnCommandBuffer*, const LvnRenderingInfo*);
    void                        (*implCmdEndRendering)(LvnCommandBuffer*);
    void                        (*implCmdBindPipeline)(LvnCommandBuffer*, LvnPipeline*);
    void                        (*implCmdSetViewport)(LvnCommandBuffer*, const LvnViewport*);
    void                        (*implCmdSetScissor)(LvnCommandBuffer*, const LvnRenderArea*);
    void                        (*implCmdDraw)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t);
    void                        (*implCmdDrawIndexed)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
    LvnResult                   (*implSurfaceAcquireNextImage)(LvnSurface*, LvnSemaphore*, LvnFence*, uint32_t*);
    LvnResult                   (*implSurfaceResize)(LvnSurface*, uint32_t, uint32_t);
    LvnResult                   (*implRenderSubmit)(const LvnGraphicsContext*, const LvnSubmitInfo*, uint32_t, LvnFence*);
    LvnResult                   (*implRenderPresent)(const LvnGraphicsContext*, const LvnPresentInfo*);
};


#endif // HG_LVN_GRAPHICS_INTERNAL_H
