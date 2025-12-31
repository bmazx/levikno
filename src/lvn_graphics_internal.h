#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "lvn_graphics.h"
#include "levikno_internal.h"


struct LvnImageView
{
    void* imageViewHandle;
};

struct LvnFramebuffer
{
    const LvnGraphicsContext* graphicsctx;
    void* framebufferData;
};

struct LvnSurface
{
    const LvnGraphicsContext* graphicsctx;
    void* surface;
    void* swapchainData;
    LvnImageView* pSwapchainImageViews;
    uint32_t swapchainImageViewCount;
    LvnFormat swapchainColorFormat;
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
    void* pipeline;
};

struct LvnCommandBuffer
{
    const LvnGraphicsContext* graphicsctx;
    void* commandbuffer;
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

    void                        (*implBeginCommandBuffer)(LvnCommandBuffer*);
    void                        (*implEndCommandBuffer)(LvnCommandBuffer*);
};


#endif // HG_LVN_GRAPHICS_INTERNAL_H
