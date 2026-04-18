#ifndef HG_LVN_INTERNAL_H
#define HG_LVN_INTERNAL_H

#include "levikno.h"


typedef struct LvnFreeNode
{
    struct LvnFreeNode* next;
} LvnFreeNode;

typedef struct LvnMemoryBlock
{
    void* block;                   /* the actual pointer to the allocation */
    uint8_t* blockAligned;         /* aligned pointer offset within the block allocation, alignment needs to be specified */
    size_t size;                   /* size of the allocation in bytes */
} LvnMemoryBlock;

typedef struct LvnMemoryPool
{
    LvnMemoryBlock memBlock;       /* the memory block */
    size_t currIndex;              /* current index to allocate the next element from the memory block */
    size_t capacity;               /* the max count of elements within the pool (not to be confused with size of block allocation in bytes) */
    size_t stride;                 /* the stride of the element in bytes in the pool (requested size) */
    size_t align;                  /* the alignment multiple of the elements in bytes */
    size_t strideAligned;          /* the stride aligned to a multiple of align (actual size alloced by pool) */
    LvnFreeNode* freeList;         /* node list of free memory indices in the pool */
    struct LvnMemoryPool* next;    /* next memory pool */

#ifdef LVN_CONFIG_DEBUG
    size_t d_AllocCount;            /* track allocations allocced from pool for debugging */
#endif
} LvnMemoryPool;

typedef struct LvnMemoryArena
{
    LvnMemoryBlock memBlock;        /* the memory block */
    size_t currIndex;               /* current index to allocate the next element from the memory block */
    size_t capacity;                /* the capacity of the user specified memory allocation in bytes (capacity may be different from block allocation size due to alignment) */
    size_t align;                   /* the alignment multiple of the allocation in bytes */
    struct LvnMemoryArena* next;    /* next memory arena */

#ifdef LVN_CONFIG_DEBUG
    size_t d_AllocCount;            /* track allocations allocced from pool for debugging */
#endif
} LvnMemoryArena;

typedef struct LvnWindowPlatformSupport
{
    bool win32Support;
    bool waylandSupport;
    bool x11Support;
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


void*              lvn_calloc(size_t size);
void               lvn_free(void* ptr);
void*              lvn_realloc(void* ptr, size_t size);

char*              lvn_strdup(const char* str);

LvnMemoryPool*     lvn_memPoolCreate(size_t count, size_t stride, size_t align);
LvnMemoryPool*     lvn_memPoolPush(LvnMemoryPool* headPool, size_t count);
void               lvn_memPoolDestroy(LvnMemoryPool* headPool);
void*              lvn_memPoolAlloc(LvnMemoryPool* memPool);
void               lvn_memPoolFree(LvnMemoryPool* memPool, void* ptr);
void               lvn_memPoolReset(LvnMemoryPool* headPool);
LvnMemoryPool*     lvn_memPoolRebuild(LvnMemoryPool* headPool);
LvnMemoryArena*    lvn_memArenaCreate(size_t size, size_t align);
LvnMemoryArena*    lvn_memArenaPush(LvnMemoryArena* headArena, size_t size);
void               lvn_memArenaDestroy(LvnMemoryArena* headArena);
void*              lvn_memArenaAlloc(LvnMemoryArena* memArena, size_t size);
void*              lvn_memArenaAllocAligned(LvnMemoryArena* memArena, size_t size, size_t align);
void               lvn_memArenaReset(LvnMemoryArena* headArena);
LvnMemoryArena*    lvn_memArenaRebuild(LvnMemoryArena* headArena);

void               lvn_getWindowPlatform(LvnWindowPlatformSupport* windowPlatformSupport);

void*              lvn_platformLoadModule(const char* path);
void               lvn_platformFreeModule(void* handle);
LvnProc            lvn_platformGetModuleSymbol(void* handle, const char* name);

#endif // !HG_LVN_INTERNAL_H
