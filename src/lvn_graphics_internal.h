#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "lvn_graphics.h"
#include "levikno_internal.h"


struct LvnSurface
{
    const LvnGraphicsContext* graphicsctx;
    void* surface;
};

struct LvnGraphicsContext
{
    LvnGraphicsApi           graphicsapi;
    const LvnContext*        ctx;
    LvnLogger*               coreLogger;
    LvnPresentationModeFlags presentModeFlags;
    bool                     enableGraphicsApiDebugLogging;

    // graphics implementation
    void*                    implData;
};


#endif // HG_LVN_GRAPHICS_INTERNAL_H
