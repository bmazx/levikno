#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H


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
} LvnContextCreateInfo;



#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult lvnCreateContext(LvnContext** ctx, LvnContextCreateInfo* createInfo);
LVN_API void      lvnDestroyContext(LvnContext* ctx);

LVN_API LvnResult lvnSetMemAllocCallbacks(LvnMemAllocFn allocFn, LvnMemFreeFn freeFn, LvnMemReallocFn reallocFn, void* userData);

#ifdef __cplusplus
}
#endif


#endif /* !HG_LEVIKNO_H */
