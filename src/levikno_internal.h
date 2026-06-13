#ifndef HG_LVN_INTERNAL_H
#define HG_LVN_INTERNAL_H

#include "levikno.h"


typedef struct LvnMemoryBlock
{
    struct LvnMemoryBlock* next;    /* ptr to the next memory block */
    uint8_t* allocation;            /* ptr to the start of the allocated block */
    uint8_t* currIndex;             /* the current index/address to be used for allocating the next allocation from the block (will be set to allocAligned on init) */
    size_t size;                    /* the size of the block (note the actual allocation may be larger due to alignment requirements, size + align) */
} LvnMemoryBlock;

typedef struct LvnFreeNode
{
    struct LvnFreeNode* next;        /* ptr to the next ptr node address in the list */
} LvnFreeNode;

typedef struct LvnArenaMark
{
    LvnMemoryBlock* block;
    struct LvnArenaMark* next;
    size_t offset;
    uint64_t generation;
} LvnArenaMark;

typedef struct LvnMemoryPool
{
    LvnMemoryBlock* blocks;         /* list of blocks containing the allocated memory */
    LvnFreeNode* freeList;          /* node list of free memory addresses in the pool */
    size_t stride;                  /* the stride of the element in bytes in the pool (requested size) */
    size_t strideAligned;           /* the stride aligned to a multiple of align (actual size allocated by pool) */
    size_t align;                   /* the alignment multiple of the elements in bytes */

    size_t allocCount;              /* number of allocations made from the pool */
} LvnMemoryPool;

typedef struct LvnMemoryArena
{
    LvnMemoryBlock* blocks;         /* list of blocks containing the allocated memory */
    size_t align;                   /* the alignment multiple of the allocation in bytes */
    uint64_t generation;            /* generation of the memory arena (increments every arena reset to prevent use of marks after reset) */
} LvnMemoryArena;

typedef struct LvnMemoryBlockCreateInfo
{
    size_t size;
    size_t align;
    LvnMemoryBlock* next;
} LvnMemoryBlockCreateInfo;

typedef struct LvnMemoryPoolCreateInfo
{
    size_t count;
    size_t stride;
    size_t align;
} LvnMemoryPoolCreateInfo;

typedef struct LvnMemoryArenaCreateInfo
{
    size_t size;
    size_t align;
} LvnMemoryArenaCreateInfo;

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

bool               lvn_ptrInBlock(uint8_t* block, size_t size, void* ptr);
LvnResult          lvn_memBlockCreate(LvnMemoryBlock** memBlock, const LvnMemoryBlockCreateInfo* createInfo);
void               lvn_memBlockDestroy(LvnMemoryBlock* memBlock);
void               lvn_memBlockDestroyChain(LvnMemoryBlock* memBlock);
size_t             lvn_memBlockGetSize(LvnMemoryBlock* memBlock);
size_t             lvn_memBlockGetOffset(LvnMemoryBlock* memBlock);
LvnResult          lvn_memPoolCreate(LvnMemoryPool* memPool, const LvnMemoryPoolCreateInfo* createInfo);
void               lvn_memPoolDestroy(LvnMemoryPool* memPool);
LvnResult          lvn_memPoolPushBlock(LvnMemoryPool* memPool, size_t count);
void*              lvn_memPoolAlloc(LvnMemoryPool* memPool);
void               lvn_memPoolFree(LvnMemoryPool* memPool, void* ptr);
void               lvn_memPoolReset(LvnMemoryPool* memPool);
LvnResult          lvn_memPoolResetMergeBlocks(LvnMemoryPool* memPool);
size_t             lvn_memPoolGetTotalCapacity(LvnMemoryPool* memPool);
size_t             lvn_memPoolGetAllocCount(LvnMemoryPool* memPool);
LvnResult          lvn_memArenaCreate(LvnMemoryArena* memArena, const LvnMemoryArenaCreateInfo* createInfo);
void               lvn_memArenaDestroy(LvnMemoryArena* memArena);
LvnResult          lvn_memArenaPushBlock(LvnMemoryArena* memArena, size_t size);
void*              lvn_memArenaAlloc(LvnMemoryArena* memArena, size_t size);
void*              lvn_memArenaAllocAligned(LvnMemoryArena* memArena, size_t size, size_t align);
LvnArenaMark       lvn_memArenaMark(LvnMemoryArena* memArena);
void               lvn_memArenaMarkRevert(LvnMemoryArena* memArena, const LvnArenaMark* mark);
LvnResult          lvn_memArenaResetMergeBlocks(LvnMemoryArena* memArena);
size_t             lvn_memArenaGetTotalSize(LvnMemoryArena* memArena);

void               lvn_getWindowPlatform(LvnWindowPlatformSupport* windowPlatformSupport);

void*              lvn_platformLoadModule(const char* path);
void               lvn_platformFreeModule(void* handle);
LvnProc            lvn_platformGetModuleSymbol(void* handle, const char* name);

#endif // !HG_LVN_INTERNAL_H
