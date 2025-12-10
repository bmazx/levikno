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
    LvnLogger          coreLogger;
    LvnLogPattern*     pUserLogPatterns;
    uint32_t           userLogPatternCount;
};

typedef void* (*LvnProc)(void);


void*     lvn_calloc(size_t size);
void      lvn_free(void* ptr);
void*     lvn_realloc(void* ptr, size_t size);

char*     lvn_strdup(const char* str);

void*     lvn_platformLoadModule(const char* path);
void      lvn_platformFreeModule(void* module);
LvnProc   lvn_platformGetModuleSymbol(void* module, const char* name);

#endif // !HG_LVN_INTERNAL_H
