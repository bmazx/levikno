#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H


// memory
#if defined(LVN_MALLOC) && defined(LVN_FREE) && defined(LVN_REALLOC)
// ok
#elif !defined(LVN_MALLOC) && !defined(LVN_FREE) && !defined(LVN_REALLOC)
// ok
#else
#error "must have all or none of LVN_MALLOC(sz), LVN_FREE(p), and LVN_REALLOC(p,sz) defined"
#endif

#ifndef LVN_MALLOC
#define LVN_MALLOC(sz) lvnMemAlloc(sz)
#endif
#ifndef LVN_FREE
#define LVN_FREE(p) lvnMemFree(p)
#endif
#ifndef LVN_REALLOC
#define LVN_REALLOC(p,sz) lvnMemRealloc(p,sz)
#endif

#include "lvn_config.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


typedef enum LvnLogLevel
{
    Lvn_LogLevel_None = 0,
    Lvn_LogLevel_Trace,
    Lvn_LogLevel_Debug,
    Lvn_LogLevel_Info,
    Lvn_LogLevel_Warn,
    Lvn_LogLevel_Error,
    Lvn_LogLevel_Fatal,
} LvnLogLevel;

typedef struct LvnContext LvnContext;
typedef struct LvnLogger LvnLogger;


typedef void* (*LvnMemAllocFn)(size_t, void*);
typedef void  (*LvnMemFreeFn)(void*, void*);
typedef void* (*LvnMemReallocFn)(void*, size_t, void*);

typedef struct LvnSink
{
    void (*logFunc)(const char*);
} LvnSink;

typedef struct LvnLogMessage
{
    const char* msg;
    const char* loggerName;
    LvnLogLevel level;
    size_t timeEpoch;
} LvnLogMessage;

typedef struct LvnLogPattern
{
    char symbol;
    char* (*func)(LvnLogMessage*);
} LvnLogPattern;

typedef struct LvnLoggerCreateContext
{
    const char* name;
    const char* format;
    LvnLogLevel level;
    const LvnSink* pSinks;
    uint32_t sinkCount;
} LvnLoggerCreateContext;

typedef struct LvnContextCreateInfo
{
    const char* appName;

    struct
    {
        bool enableLogging;
        bool enableCoreLogging;
        const char* coreLogFormat;
        LvnLogLevel coreLogLevel;
        const LvnSink* pCoreSinks;
        uint32_t coreSinkCount;
    } logging;

    struct
    {
        LvnMemAllocFn memAllocCallback;
        LvnMemFreeFn memFreeCallback;
        LvnMemReallocFn memReallocCallback;
        void* memAllocUserData;
    } memory;
} LvnContextCreateInfo;



#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult lvnCreateContext(LvnContext** ctx, LvnContextCreateInfo* createInfo);
LVN_API void      lvnDestroyContext(LvnContext* ctx);

LVN_API void*     lvnMemAlloc(size_t size);
LVN_API void      lvnMemFree(void* ptr);
LVN_API void*     lvnMemRealloc(void* ptr, size_t size);

LVN_API void*     lvnCtxMemAlloc(LvnContext* ctx, size_t size);
LVN_API void      lvnCtxMemFree(LvnContext* ctx, void* ptr);
LVN_API size_t    lvnCtxGetMemAllocCount(LvnContext* ctx);

#ifdef __cplusplus
}
#endif


#endif /* !HG_LEVIKNO_H */
