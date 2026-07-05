#ifndef HG_LVN_INTERNAL_H
#define HG_LVN_INTERNAL_H

#include "levikno.h"


typedef struct LvnWindowPlatformSupport
{
    bool win32;
    bool wayland;
    bool x11;
} LvnWindowPlatformSupport;

struct LvnLogger
{
    const LvnContext*    ctx;                 /* pointer to context */
    char*                loggerName;          /* the name of the logger */
    char*                logPatternFormat;    /* string representation of log pattern format */
    LvnLogLevel          logLevel;            /* log level enum */
    LvnLogPattern*       pLogPatterns;        /* array containing the log patterns */
    uint32_t             logPatternCount;     /* size of log patterns array */
    LvnSink*             pSinks;              /* array containing the sinks */
    uint32_t             sinkCount;           /* size of the sink array */
    bool                 logging;             /* whether the logger should log to the sinks or not */
};

struct LvnContext
{
    char*             appName;
    LvnLogger         coreLogger;             /* the core logger for the context */
    LvnLogPattern*    pUserLogPatterns;       /* array of log patterns for the core logger */
    uint32_t          userLogPatternCount;    /* number of log patterns in the array */
    bool              enableLogging;          /* enable/disable logging for all loggers created from the context */
};

typedef void* (*LvnProc)(void);


void*                    lvn_calloc(size_t size);
void                     lvn_free(void* ptr);
void*                    lvn_realloc(void* ptr, size_t size);

char*                    lvn_strdup(const char* str);

LvnWindowPlatformSupport lvn_getWindowPlatform(void);

void*                    lvn_platformLoadModule(const char* path);
void                     lvn_platformFreeModule(void* handle);
LvnProc                  lvn_platformGetModuleSymbol(void* handle, const char* name);

#endif // !HG_LVN_INTERNAL_H
