#include "levikno_internal.h"

#include <stdlib.h>
#include <string.h>

#define LVN_ABORT(rc) exit(rc)


static void*   mallocWrapper(size_t size, void* userData)               { (void)userData; return malloc(size); }
static void    freeWrapper(void* ptr, void* userData)                   { (void)userData; free(ptr); }
static void*   reallocWrapper(void* ptr, size_t size, void* userData)   { (void)userData; return realloc(ptr, size); }


LvnResult lvnCreateContext(LvnContext** ctx, LvnContextCreateInfo* createInfo)
{
    if (!ctx)
        return Lvn_Result_Failure;

    if (createInfo && createInfo->memory.memAllocCallback)
        *ctx = createInfo->memory.memAllocCallback(sizeof(LvnContext), createInfo->memory.memAllocUserData);
    else
        *ctx = LVN_MALLOC(sizeof(LvnContext));

    if (!*ctx)
        return Lvn_Result_Failure;

    memset(*ctx, 0, sizeof(LvnContext));
    LvnContext* ctxPtr = *ctx;

    if (createInfo)
    {
        ctxPtr->enableLogging = createInfo->logging.enableLogging;
        ctxPtr->enableCoreLogging = createInfo->logging.enableCoreLogging;

        ctxPtr->memAllocCallback = createInfo->memory.memAllocCallback;
        ctxPtr->memFreeCallback = createInfo->memory.memFreeCallback;
        ctxPtr->memReallocCallback = createInfo->memory.memReallocCallback;
        ctxPtr->memAllocUserData = createInfo->memory.memAllocUserData;
    }
    else
    {
        ctxPtr->enableLogging = true;
        ctxPtr->enableCoreLogging = true;

        ctxPtr->memAllocCallback = mallocWrapper;
        ctxPtr->memFreeCallback = freeWrapper;
        ctxPtr->memReallocCallback = reallocWrapper;
        ctxPtr->memAllocUserData = NULL;
    }

    return Lvn_Result_Success;
}

void lvnDestroyContext(LvnContext* ctx)
{
    if (!ctx)
        return;

    if (ctx->memFreeCallback)
        ctx->memFreeCallback(ctx, ctx->memAllocUserData);
    else
        LVN_FREE(ctx);
}

void* lvnMemAlloc(size_t size)
{
    return mallocWrapper(size, NULL);
}

void lvnMemFree(void* ptr)
{
    freeWrapper(ptr, NULL);
}

void* lvnMemRealloc(void* ptr, size_t size)
{
    return reallocWrapper(ptr, size, NULL);
}

void* lvnCtxMemAlloc(LvnContext* ctx, size_t size)
{
    if (!ctx || size == 0) { return NULL; }
    void* allocmem = ctx->memAllocCallback(size, ctx->memAllocUserData);
    // if (!allocmem) { LVN_CORE_ERROR("malloc failure, could not allocate memory!"); LVN_ABORT; }
    memset(allocmem, 0, size);
    ctx->memAllocCount++;
    return allocmem;
}

void lvnCtxMemFree(LvnContext* ctx, void* ptr)
{
    if (!ctx || !ptr) { return; }
    ctx->memFreeCallback(ptr, ctx->memAllocUserData);
    ctx->memAllocCount--;
}

size_t lvnCtxGetMemAllocCount(LvnContext* ctx)
{
    LVN_ASSERT(ctx, "ctx is nullptr");
    return ctx->memAllocCount;
}
