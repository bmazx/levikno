#include "levikno.h"


struct LvnLogger
{
    char* loggerName;
    char* logPatternFormat;
    LvnLogLevel logLevel;
    LvnLogPattern* pLogPatterns;
    uint32_t logPatternCount;
    LvnSink* pSinks;
    uint32_t sinkCount;
};

struct LvnContext
{
    bool               enableLogging;
    bool               enableCoreLogging;
    LvnLogger          coreLogger;
};


void*     lvn_calloc(size_t size);
void      lvn_free(void* ptr);
void*     lvn_realloc(void* ptr, size_t size);

char*     lvn_strdup(const char* str);
