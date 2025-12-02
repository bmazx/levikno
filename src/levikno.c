#include "levikno_internal.h"

#include <stdlib.h>
#include <string.h>

#define LVN_DEFAULT_LOG_PATTERN "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"
#define LVN_ABORT(rc) exit(rc)


static void*   mallocWrapper(size_t size, void* userData)               { (void)userData; return malloc(size); }
static void    freeWrapper(void* ptr, void* userData)                   { (void)userData; free(ptr); }
static void*   reallocWrapper(void* ptr, size_t size, void* userData)   { (void)userData; return realloc(ptr, size); }
static LvnMemAllocFn s_LvnMemAllocFnCallback = mallocWrapper;
static LvnMemFreeFn s_LvnMemFreeFnCallback = freeWrapper;
static LvnMemReallocFn s_LvnMemReallocFnCallback = reallocWrapper;
static void* s_LvnMemUserData = NULL;


LvnResult lvnCreateContext(LvnContext** ctx, LvnContextCreateInfo* createInfo)
{
    if (!ctx)
        return Lvn_Result_Failure;

    *ctx = lvn_calloc(sizeof(LvnContext));

    if (!*ctx)
        return Lvn_Result_Failure;

    memset(*ctx, 0, sizeof(LvnContext));
    LvnContext* ctxPtr = *ctx;

    if (createInfo)
    {
        ctxPtr->enableLogging = createInfo->logging.enableLogging;
        ctxPtr->enableCoreLogging = createInfo->logging.enableCoreLogging;
    }
    else
    {
        ctxPtr->enableLogging = true;
        ctxPtr->enableCoreLogging = true;
    }

    if (createInfo && createInfo->appName)
        ctxPtr->coreLogger.loggerName = lvn_strcon(createInfo->appName);
    else
        ctxPtr->coreLogger.loggerName = lvn_strcon("CORE");

    if (createInfo && createInfo->logging.coreLogFormat)
        ctxPtr->coreLogger.logPatternFormat = lvn_strcon(createInfo->logging.coreLogFormat);
    else
        ctxPtr->coreLogger.logPatternFormat = lvn_strcon(LVN_DEFAULT_LOG_PATTERN);

    if (createInfo && createInfo->logging.coreLogLevel)
        ctxPtr->coreLogger.logLevel = createInfo->logging.coreLogLevel;
    else
        ctxPtr->coreLogger.logLevel = Lvn_LogLevel_None;

    if (createInfo && createInfo->logging.pCoreSinks)
    {
        ctxPtr->coreLogger.pSinks = lvn_calloc(sizeof(LvnSink) * createInfo->logging.coreSinkCount);
        memcpy(ctxPtr->coreLogger.pSinks, createInfo->logging.pCoreSinks, sizeof(LvnSink) * createInfo->logging.coreSinkCount);
        ctxPtr->coreLogger.sinkCount = createInfo->logging.coreSinkCount;
    }
    else
    {
        ctxPtr->coreLogger.pSinks = lvn_calloc(sizeof(LvnSink));
        ctxPtr->coreLogger.pSinks->logFunc = NULL;
        ctxPtr->coreLogger.sinkCount = 1;
    }

    return Lvn_Result_Success;
}

void lvnDestroyContext(LvnContext* ctx)
{
    if (!ctx)
        return;

    lvn_free(ctx->coreLogger.loggerName);
    lvn_free(ctx->coreLogger.logPatternFormat);
    lvn_free(ctx->coreLogger.pLogPatterns);
    lvn_free(ctx->coreLogger.pSinks);

    lvn_free(ctx);
}

LvnResult lvnSetMemAllocCallbacks(LvnMemAllocFn allocFn, LvnMemFreeFn freeFn, LvnMemReallocFn reallocFn, void* userData)
{
    if (!allocFn || ! !freeFn || !reallocFn)
        return Lvn_Result_Failure;

    s_LvnMemAllocFnCallback = allocFn;
    s_LvnMemFreeFnCallback = freeFn;
    s_LvnMemReallocFnCallback = reallocFn;
    s_LvnMemUserData = userData;

    return Lvn_Result_Success;
}

void* lvn_calloc(size_t size)
{
    void* result = s_LvnMemAllocFnCallback(size, s_LvnMemUserData);
    if (!result) { return NULL; }
    memset(result, 0, size);
    return result;
}

void lvn_free(void* ptr)
{
    s_LvnMemFreeFnCallback(ptr, s_LvnMemUserData);
}

void* lvn_realloc(void* ptr, size_t size)
{
    return s_LvnMemReallocFnCallback(ptr, size, s_LvnMemUserData);
}

char* lvn_strcon(const char* str)
{
    const size_t length = strlen(str) + 1;
    char* result = lvn_calloc(length);
    memcpy(result, str, length);
    return result;
}
