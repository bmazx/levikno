#ifndef HG_LVN_INTERNAL_H
#define HG_LVN_INTERNAL_H

#include "levikno.h"


typedef struct LvnFreeNode
{
    struct LvnFreeNode* next;
} LvnFreeNode;

typedef struct LvnMemoryBlock
{
    void* block;
    size_t size;
} LvnMemoryBlock;

typedef struct LvnMemoryPool
{
    LvnMemoryBlock* memBlock;      /* the memory block */
    size_t currIndex;              /* current index to allocate the next element from the memory block */
    size_t capacity;               /* the max count of elements within the pool */
    size_t stride;                 /* the stride of the element in bytes in the pool */
    size_t align;                  /* the alignment multiple of the elements in bytes */
    size_t strideAligned;          /* the stride aligned to a multiple of align */
    size_t nextAllocCount;         /* the alloc size count for the next memory block */
    LvnFreeNode* freeList;         /* node list of free memory indices in the pool */
    struct LvnMemoryPool* next;    /* next memory pool */
} LvnMemoryPool;

typedef struct LvnMemoryArena
{
    LvnMemoryBlock* memBlock;       /* the memory block */
    size_t currIndex;               /* current index to allocate the next element from the memory block */
    size_t capacity;                /* the capacity of the memory allocation in bytes */
    size_t align;                   /* the alignment multiple of the allocation in bytes */
    size_t nextAllocSize;           /* the alloc size for the next memory allocation */
    struct LvnMemoryArena* next;    /* next memory arena */
} LvnMemoryArena;

struct LvnLogger
{
    const LvnContext*    ctx;
    char*                loggerName;
    char*                logPatternFormat;
    LvnLogLevel          logLevel;
    LvnLogPattern*       pLogPatterns;
    uint32_t             logPatternCount;
    LvnSink*             pSinks;
    uint32_t             sinkCount;
    bool                 logging;
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

LvnMemoryBlock*    lvn_memBlockAlloc(size_t size);
void               lvn_memBlockFree(LvnMemoryBlock* headBlock);
bool               lvn_memBlockPtrInBlock(LvnMemoryBlock* memBlock, void* ptr);
LvnMemoryPool*     lvn_memPoolCreate(size_t count, size_t stride, size_t align, size_t nextAllocCount);
LvnMemoryPool*     lvn_memPoolPush(LvnMemoryPool* headPool, size_t count, size_t nextAllocCount);
void               lvn_memPoolDestroy(LvnMemoryPool* headPool);
void*              lvn_memPoolAlloc(LvnMemoryPool* memPool);
void               lvn_memPoolFree(LvnMemoryPool* memPool, void* ptr);
LvnMemoryArena*    lvn_memArenaCreate(size_t size, size_t align, size_t nextAllocSize);
LvnMemoryArena*    lvn_memArenaPush(LvnMemoryArena* headArena, size_t size, size_t nextAllocSize);
void               lvn_memArenaDestroy(LvnMemoryArena* headArena);
void*              lvn_memArenaAlloc(LvnMemoryArena* memArena, size_t size);
void               lvn_memArenaReset(LvnMemoryArena* headArena);
LvnMemoryArena*    lvn_memArenaResetGlob(LvnMemoryArena* headArena);

void*              lvn_platformLoadModule(const char* path);
void               lvn_platformFreeModule(void* handle);
LvnProc            lvn_platformGetModuleSymbol(void* handle, const char* name);

#endif // !HG_LVN_INTERNAL_H
