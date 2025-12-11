#ifndef HG_LVN_INTERNAL_H
#define HG_LVN_INTERNAL_H

#include "levikno.h"


struct LvnLogger
{
    const LvnContext* ctx;
    char* loggerName;
    char* logPatternFormat;
    LvnLogLevel logLevel;
    LvnLogPattern* pLogPatterns;
    uint32_t logPatternCount;
    LvnSink* pSinks;
    uint32_t sinkCount;
    bool logging;
};

struct LvnContext
{
    LvnLogger          coreLogger;            // the core logger for the context
    LvnLogPattern*     pUserLogPatterns;      // array of log patterns for the core logger
    uint32_t           userLogPatternCount;   // number of log patterns in the array
    bool               enableLogging;         // enable/disable logging for all loggers created from the context
};

typedef void* (*LvnProc)(void);


void*     lvn_calloc(size_t size);
void      lvn_free(void* ptr);
void*     lvn_realloc(void* ptr, size_t size);

char*     lvn_strdup(const char* str);

void*     lvn_platformLoadModule(const char* path);
void      lvn_platformFreeModule(void* handle);
LvnProc   lvn_platformGetModuleSymbol(void* handle, const char* name);

#endif // !HG_LVN_INTERNAL_H
