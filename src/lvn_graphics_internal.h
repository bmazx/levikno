#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "lvn_graphics.h"
#include "levikno_internal.h"


struct LvnRenderPass
{
    void* renderPassHandle;
};

struct LvnSurface
{
    const LvnGraphicsContext* graphicsctx;
    void* surface;
    void* swapchainData;
    LvnRenderPass renderPass;
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

struct LvnGraphicsContext
{
    LvnGraphicsApi            graphicsapi;
    const LvnContext*         ctx;
    LvnLogger*                coreLogger;
    LvnPresentationModeFlags  presentModeFlags;
    bool                      enableGraphicsApiDebugLogging;

    // graphics implementation
    void*                     implData;
    LvnResult                 (*implCreateSurface)(const LvnGraphicsContext*, LvnSurface*, const LvnSurfaceCreateInfo*);
    void                      (*implDestroySurface)(LvnSurface*);
    LvnResult                 (*implCreateShader)(const LvnGraphicsContext*, LvnShader*, const LvnShaderCreateInfo*);
    void                      (*implDestroyShader)(LvnShader*);
    LvnResult                 (*implCreatePipeline)(const LvnGraphicsContext*, LvnPipeline*, const LvnPipelineCreateInfo*);
    void                      (*implDestroyPipeline)(LvnPipeline*);
};


#endif // HG_LVN_GRAPHICS_INTERNAL_H
