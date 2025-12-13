#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "lvn_graphics.h"
#include "levikno_internal.h"


struct LvnGraphicsContext
{
    LvnGraphicsApi    graphicsapi;
    const LvnContext* ctx;
    bool              enableGraphicsApiDebugLogging;
    LvnLogger*        coreLogger;
    void*             implData;

    LvnResult         (*implInit)(LvnGraphicsContext*, const LvnGraphicsContextCreateInfo*);
    void              (*implTerminate)(LvnGraphicsContext*);
};


#endif // HG_LVN_GRAPHICS_INTERNAL_H
