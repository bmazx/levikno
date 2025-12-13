#include "lvn_graphics_internal.h"

#ifdef LVN_INCLUDE_VULKAN
#include "lvn_impl_vk.h"
#endif

static const char* lvn_getGraphicsApiEnumName(LvnGraphicsApi api);


static const char* lvn_getGraphicsApiEnumName(LvnGraphicsApi api)
{
    switch (api)
    {
        case Lvn_GraphicsApi_None:   { return "none"; }
        case Lvn_GraphicsApi_Opengl: { return "opengl"; }
        case Lvn_GraphicsApi_Vulkan: { return "vulkan"; }
    }

    LVN_ASSERT(false, "api in not a valid enum value");
    return NULL;
}

LvnResult lvnCreateGraphicsContext(struct LvnContext* ctx, LvnGraphicsContext** graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    if (!graphicsctx || !createInfo)
        return Lvn_Result_Failure;

    *graphicsctx = (LvnGraphicsContext*) lvn_calloc(sizeof(LvnGraphicsContext));

    if (!*graphicsctx)
        return Lvn_Result_Failure;

    LvnGraphicsContext* gctxPtr = *graphicsctx;
    gctxPtr->graphicsapi = createInfo->graphicsapi;
    gctxPtr->ctx = ctx;
    gctxPtr->enableGraphicsApiDebugLogging = createInfo->enableGraphicsApiDebugLogging;
    gctxPtr->coreLogger = &ctx->coreLogger;

    LvnResult result = Lvn_Result_Success;
    switch (createInfo->graphicsapi)
    {
        case Lvn_GraphicsApi_None:
            break;
        case Lvn_GraphicsApi_Opengl:
            // TODO: add opengl impl
            break;
        case Lvn_GraphicsApi_Vulkan:
#ifdef LVN_INCLUDE_VULKAN
            result = lvnImplVkInit(gctxPtr, createInfo);
#endif
            break;
    }

    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(gctxPtr->coreLogger, "failed to create graphics context, graphics api: %s",
                      lvn_getGraphicsApiEnumName(createInfo->graphicsapi));
        return result;
    }

    LVN_LOG_TRACE(gctxPtr->coreLogger, "graphics context created: (%p), graphics api set: %s",
                  *graphicsctx,
                  lvn_getGraphicsApiEnumName(createInfo->graphicsapi));

    return Lvn_Result_Success;
}

void lvnDestroyGraphicsContext(LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    switch (graphicsctx->graphicsapi)
    {
        case Lvn_GraphicsApi_None:
            break;
        case Lvn_GraphicsApi_Opengl:
            // TODO: add opengl impl
            break;
        case Lvn_GraphicsApi_Vulkan:
#ifdef LVN_INCLUDE_VULKAN
            lvnImplVkTerminate(graphicsctx);
#endif
            break;
    }

    LVN_LOG_TRACE(graphicsctx->coreLogger, "graphics context terminated: (%p)", graphicsctx);

    lvn_free(graphicsctx);
}
