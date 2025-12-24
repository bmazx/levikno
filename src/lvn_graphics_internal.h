#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "lvn_graphics.h"
#include "levikno_internal.h"


struct LvnSurface
{
    const LvnGraphicsContext* graphicsctx;
    void* surface;
    void* swapchainData;
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
    LvnPipelineFixedFunctions defaultPipelineFixedFuncs;

    // graphics implementation
    void*                     implData;
    LvnResult                 (*implCreateSurface)(const LvnGraphicsContext*, LvnSurface*, const LvnSurfaceCreateInfo*);
    void                      (*implDestroySurface)(LvnSurface*);
};


#endif // HG_LVN_GRAPHICS_INTERNAL_H
