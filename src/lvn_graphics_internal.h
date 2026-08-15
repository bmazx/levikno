#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "lvn_graphics.h"

#include "levikno_internal.h"
#include "lvn_cma.h"


struct LvnBuffer
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        bufferData;
    uint64_t                     size;
    LvnBufferTypeFlags           type;
    LvnBufferMemoryUsage         usage;
};

struct LvnSampler
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        samplerData;
};

struct LvnTexture
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        textureData;
    uint32_t                     width;
    uint32_t                     height;
};

struct LvnRenderPass
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        renderpassData;
};

struct LvnFramebuffer
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        framebufferData;
};

struct LvnSurface
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        surfaceData;
};

struct LvnSwapchain
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        swapchainData;
    LvnTexture*                  pSwapchainImages;
    uint32_t                     swapchainImageCount;
    LvnFormat                    swapchainColorFormat;
    LvnExtent2D                  extent;
};

struct LvnDescriptorLayout
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        descriptorLayoutData;
};

struct LvnDescriptorPool
{
    const LvnGraphicsContext*    graphicsctx;
    LvnMemoryPool                setPool;
    void*                        descriptorPoolData;
};

struct LvnDescriptorSet
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        descriptorSetData;
};

struct LvnShader
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        shaderData;
};

struct LvnPipeline
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        pipelineData;
};

struct LvnCommandBuffer
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        commandbufferData;
    LvnMemoryArena               frameArena;
};

struct LvnFence
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        fenceData;
};

struct LvnSemaphore
{
    const LvnGraphicsContext*    graphicsctx;
    void*                        semaphoreData;
};

struct LvnGraphicsContext
{
    LvnGraphicsApi                            graphicsapi;
    const LvnContext*                         ctx;
    const LvnLogger*                          coreLogger;
    LvnPresentationModeFlags                  presentModeFlags;
    bool                                      enableGraphicsApiDebugLogging;

    struct
    {
        size_t                                baseFrameArenaSize;
        size_t                                baseCmdBuffFrameArenaSize;
        size_t                                baseCmdBuffByteStreamSize;
    } memory;

    // graphics implementation
    void*                                     implData;
    PFN_lvnGetSurface                         implGetSurface;
    PFN_lvnCreateSurface                      implCreateSurface;
    PFN_lvnDestroySurface                     implDestroySurface;
    PFN_lvnCreateSwapchain                    implCreateSwapchain;
    PFN_lvnDestroySwapchain                   implDestroySwapchain;
    PFN_lvnCreateRenderPass                   implCreateRenderPass;
    PFN_lvnDestroyRenderPass                  implDestroyRenderPass;
    PFN_lvnCreateFramebuffer                  implCreateFramebuffer;
    PFN_lvnDestroyFramebuffer                 implDestroyFramebuffer;
    PFN_lvnCreateShader                       implCreateShader;
    PFN_lvnDestroyShader                      implDestroyShader;
    PFN_lvnCreateDescriptorLayout             implCreateDescriptorLayout;
    PFN_lvnDestroyDescriptorLayout            implDestroyDescriptorLayout;
    PFN_lvnCreateDescriptorPool               implCreateDescriptorPool;
    PFN_lvnDestroyDescriptorPool              implDestroyDescriptorPool;
    PFN_lvnCreatePipeline                     implCreatePipeline;
    PFN_lvnDestroyPipeline                    implDestroyPipeline;
    PFN_lvnCreateFence                        implCreateFence;
    PFN_lvnDestroyFence                       implDestroyFence;
    PFN_lvnCreateSemaphore                    implCreateSemaphore;
    PFN_lvnDestroySemaphore                   implDestroySemaphore;
    PFN_lvnCreateBuffer                       implCreateBuffer;
    PFN_lvnDestroyBuffer                      implDestroyBuffer;
    PFN_lvnCreateSampler                      implCreateSampler;
    PFN_lvnDestroySampler                     implDestroySampler;
    PFN_lvnCreateTexture                      implCreateTexture;
    PFN_lvnDestroyTexture                     implDestroyTexture;
    PFN_lvnCreateCommandBuffer                implCreateCommandBuffer;
    PFN_lvnDestroyCommandBuffer               implDestroyCommandBuffer;
    PFN_lvnAllocateDescriptorSets             implAllocateDescriptorSets;
    PFN_lvnResetDescriptorPool                implResetDescriptorPool;
    PFN_lvnUpdateDescriptorSets               implUpdateDescriptorSets;
    PFN_lvnSurfaceGetSupportedFormats         implSurfaceGetSupportedFormats;
    PFN_lvnSurfaceGetSupportedPresentModes    implSurfaceGetSupportedPresentModes;
    PFN_lvnSwapchainResize                    implSwapchainResize;
    PFN_lvnSwapchainAcquireNextImage          implSwapchainAcquireNextImage;
    PFN_lvnFenceWait                          implFenceWait;
    PFN_lvnFenceReset                         implFenceReset;
    PFN_lvnBufferUpdate                       implBufferUpdate;
    PFN_lvnBeginCommandBuffer                 implBeginCommandBuffer;
    PFN_lvnEndCommandBuffer                   implEndCommandBuffer;
    PFN_lvnCmdBeginRenderPass                 implCmdBeginRenderPass;
    PFN_lvnCmdEndRenderPass                   implCmdEndRenderPass;
    PFN_lvnCmdBindPipeline                    implCmdBindPipeline;
    PFN_lvnCmdBindVertexBuffer                implCmdBindVertexBuffer;
    PFN_lvnCmdBindIndexBuffer                 implCmdBindIndexBuffer;
    PFN_lvnCmdBindDescriptorSets              implCmdBindDescriptorSets;
    PFN_lvnCmdSetViewport                     implCmdSetViewport;
    PFN_lvnCmdSetScissor                      implCmdSetScissor;
    PFN_lvnCmdDraw                            implCmdDraw;
    PFN_lvnCmdDrawIndexed                     implCmdDrawIndexed;
    PFN_lvnRenderSubmit                       implRenderSubmit;
    PFN_lvnRenderPresent                      implRenderPresent;
};


const char* lvn_getShaderStageEnumName(LvnShaderStageFlagBits stage);


#endif // !HG_LVN_GRAPHICS_INTERNAL_H
