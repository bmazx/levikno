#ifndef HG_LVN_IMPL_OGL_H
#define HG_LVN_IMPL_OGL_H

#include "lvn_graphics_internal.h"
#include "levikno_internal.h"


typedef struct LvnOpenglLoader
{
    void* loaderHandle;
} LvnOpenglLoader;

typedef struct LvnOpenglBackends
{
    void* handle;

    const LvnGraphicsContext* graphicsctx;
    LvnOpenglLoader oglLoader;
} LvnOpenglBackends;

LvnResult lvnImplOglInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo);
void      lvnImplOglTerminate(LvnGraphicsContext* graphicsctx);

#endif // !HG_LVN_IMPL_OGL_H
