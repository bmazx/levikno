#include "lvn_graphics_internal.h"


LvnResult lvnCreateGraphicsContext(struct LvnContext* ctx, LvnGraphicsContext** graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    if (!graphicsctx || !createInfo)
        return Lvn_Result_Failure;

    *graphicsctx = (LvnGraphicsContext*) lvn_calloc(sizeof(LvnGraphicsContext));

    if (!*graphicsctx)
        return Lvn_Result_Failure;

    LvnGraphicsContext* gctxPtr = *graphicsctx;
    gctxPtr->graphicsapi = createInfo->graphicsapi;
    gctxPtr->coreLogger = &ctx->coreLogger;

    return Lvn_Result_Success;
}

void lvnDestroyGraphicsContext(LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    lvn_free(graphicsctx);
}
