#include "lvn_audio_internal.h"


LvnResult lvnCreateAudioContext(struct LvnContext* ctx, LvnAudioContext** audioctx, const LvnAudioContextCreateInfo* createInfo)
{
    LVN_ASSERT(ctx && audioctx && createInfo, "ctx, audioctx, and createInfo cannot be null");

    LvnResult result = Lvn_Result_Success;

    // create and init graphics context
    *audioctx = (LvnAudioContext*) lvn_calloc(sizeof(LvnAudioContext));

    if (!*audioctx)
    {
        LVN_LOG_ERROR(&ctx->coreLogger, "failed to allocate memory for graphics context");
        result = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnAudioContext* actxPtr = *audioctx;
    actxPtr->ctx = ctx;
    actxPtr->coreLogger = &ctx->coreLogger;

    return Lvn_Result_Success;

fail_cleanup:
    if (*audioctx)
    {
        lvnDestroyAudioContext(*audioctx);
        *audioctx = NULL;
    }
    return result;
}

void lvnDestroyAudioContext(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
}
