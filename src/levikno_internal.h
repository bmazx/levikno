#include "levikno.h"


struct LvnLogger
{
    char* loggerName;
    char* logPatternFormat;
    LvnLogLevel logLevel;
    LvnLogPattern* logPatterns;
    uint32_t logPatternCount;
};

struct LvnContext
{
    bool               enableLogging;
    bool               enableCoreLogging;
    LvnLogger          coreLogger;

    LvnMemAllocFn      memAllocCallback;
    LvnMemFreeFn       memFreeCallback;
    LvnMemReallocFn    memReallocCallback;
    void*              memAllocUserData;

    size_t             memAllocCount;
};
