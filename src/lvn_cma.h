#ifndef LVN_CUSTOM_MEMORY_ALLOCATOR_H
#define LVN_CUSTOM_MEMORY_ALLOCATOR_H

// debug
#ifdef LVN_CONFIG_DEBUG
    #ifndef LVN_DEBUG_ALLOC_VALUE
        #define LVN_DEBUG_ALLOC_VALUE (0xCD)
    #endif
    #ifndef LVN_DEBUG_FREE_VALUE
        #define LVN_DEBUG_FREE_VALUE (0xDD)
    #endif
#endif

#ifndef LVN_CMA_ASSERT
    #define LVN_CMA_ASSERT(x, str) assert((x) && str)
#endif

// memory
#if defined(LVN_CMA_MALLOC) && defined(LVN_CMA_FREE)
// ok
#elif !defined(LVN_CMA_MALLOC) && !defined(LVN_CMA_FREE)
    #define LVN_CMA_MALLOC(sz)    malloc(sz)
    #define LVN_CMA_FREE(ptr)     free(ptr)
#else
    #error "must define both LVN_CMA_MALLOC and LVN_CMA_FREE or neither"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// alignment
#if defined(__cplusplus) || (defined(__STDC__) && (__STDC_VERSION__ >= 202311L))
    #ifndef LVN_ALIGNOF
        #define LVN_ALIGNOF(T) alignof(T)
    #endif
    typedef max_align_t lvn_max_align_t;
#elif defined(__STDC__) && (__STDC_VERSION__ >= 201112L)
    #ifndef LVN_ALIGNOF
        #define LVN_ALIGNOF(T) _Alignof(T)
    #endif
    typedef max_align_t lvn_max_align_t;
#else
    #ifndef LVN_ALIGNOF
        #define LVN_ALIGNOF(T) ((size_t)offsetof(struct { char c; T x; }, x))
    #endif
    typedef union {
        long long ll;
        long double ld;
        void* p;
    } lvn_max_align_t;
#endif

#ifndef LVN_DEFAULT_ALIGN
    #define LVN_DEFAULT_ALIGN (LVN_ALIGNOF(lvn_max_align_t))
#endif
#ifndef LVN_ALIGN_UP
    #define LVN_ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))
#endif
#ifndef LVN_ALIGN_DOWN
    #define LVN_ALIGN_DOWN(x, a) ((x) & ~((a) - 1))
#endif
#ifndef LVN_ALIGNED
    #define LVN_ALIGNED(x, a) (((x) & ((a) - 1)) == 0)
#endif

// error
#define LVN_CMA_SUCCESS (0)
#define LVN_CMA_ERROR (-1)
#define LVN_CMA_ALLOC_ERROR (-2)

typedef uint32_t LvnCmaFlags;

typedef enum LvnMemoryPoolFlagBits
{
    Lvn_MemoryPoolFlag_DynamicGrowth = 0x00000001,
} LvnMemoryPoolFlagBits;
typedef LvnCmaFlags LvnMemoryPoolFlags;

typedef enum LvnMemoryArenaFlagBits
{
    Lvn_MemoryArenaFlag_DynamicGrowth = 0x00000001,
} LvnMemoryArenaFlagBits;
typedef LvnCmaFlags LvnMemoryArenaFlags;

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
    size_t offset;
    uint64_t generation;
} LvnArenaMark;

typedef struct LvnMemoryPool
{
    LvnMemoryBlock* front;          /* list of blocks starting from the first block */
    LvnMemoryBlock* back;           /* last block in the list, always the current/active block */
    LvnFreeNode* freeList;          /* node list of free memory addresses in the pool */
    size_t stride;                  /* the stride of the element in bytes in the pool (requested size) */
    size_t strideAligned;           /* the stride aligned to a multiple of align (actual size allocated by pool) */
    size_t align;                   /* the alignment multiple of the elements in bytes */
    LvnMemoryPoolFlags flags;       /* flag bits for the memory pool */

    size_t allocCount;              /* number of allocations made from the pool */
} LvnMemoryPool;

typedef struct LvnMemoryArena
{
    LvnMemoryBlock* front;          /* list of blocks starting from the first block */
    LvnMemoryBlock* back;           /* last block in the list, always the current/active block */
    size_t align;                   /* the alignment multiple of the allocation in bytes */
    LvnMemoryArenaFlags flags;      /* flag bits for the memory arena */

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
    LvnMemoryPoolFlags flags;
} LvnMemoryPoolCreateInfo;

typedef struct LvnMemoryArenaCreateInfo
{
    size_t size;
    size_t align;
    LvnMemoryArenaFlags flags;
} LvnMemoryArenaCreateInfo;


#ifdef __cplusplus
extern "C" {
#endif

bool            lvn_ptrInBlock(uint8_t* block, size_t size, void* ptr);
int             lvn_memBlockCreate(LvnMemoryBlock** memBlock, const LvnMemoryBlockCreateInfo* createInfo);
void            lvn_memBlockDestroy(LvnMemoryBlock* memBlock);
void            lvn_memBlockDestroyChain(LvnMemoryBlock* memBlock);
size_t          lvn_memBlockGetSize(LvnMemoryBlock* memBlock);
size_t          lvn_memBlockGetOffset(LvnMemoryBlock* memBlock);
int             lvn_memPoolCreate(LvnMemoryPool* memPool, const LvnMemoryPoolCreateInfo* createInfo);
void            lvn_memPoolDestroy(LvnMemoryPool* memPool);
int             lvn_memPoolPushBlock(LvnMemoryPool* memPool, size_t count);
void*           lvn_memPoolAlloc(LvnMemoryPool* memPool);
void            lvn_memPoolFree(LvnMemoryPool* memPool, void* ptr);
void            lvn_memPoolReset(LvnMemoryPool* memPool);
int             lvn_memPoolResetMergeBlocks(LvnMemoryPool* memPool);
size_t          lvn_memPoolGetTotalCapacity(LvnMemoryPool* memPool);
size_t          lvn_memPoolGetAllocCount(LvnMemoryPool* memPool);
int             lvn_memArenaCreate(LvnMemoryArena* memArena, const LvnMemoryArenaCreateInfo* createInfo);
void            lvn_memArenaDestroy(LvnMemoryArena* memArena);
int             lvn_memArenaPushBlock(LvnMemoryArena* memArena, size_t size);
void*           lvn_memArenaAlloc(LvnMemoryArena* memArena, size_t size);
void*           lvn_memArenaAllocAligned(LvnMemoryArena* memArena, size_t size, size_t align);
LvnArenaMark    lvn_memArenaMark(LvnMemoryArena* memArena);
void            lvn_memArenaMarkRevert(LvnMemoryArena* memArena, const LvnArenaMark* mark);
int             lvn_memArenaResetMergeBlocks(LvnMemoryArena* memArena);
size_t          lvn_memArenaGetTotalSize(LvnMemoryArena* memArena);
LvnMemoryBlock* lvn_memArenaGetCurrBlock(LvnMemoryArena* memArena);

#ifdef __cplusplus
}
#endif


#ifdef LVN_CMA_IMPL

#include <assert.h>
#include <stdlib.h>
#include <string.h>


bool lvn_ptrInBlock(uint8_t* block, size_t size, void* ptr)
{
    uint8_t* start = block;
    uint8_t* end = start + size;
    return (uint8_t*)ptr >= start && (uint8_t*)ptr < end;
}

int lvn_memBlockCreate(LvnMemoryBlock** memBlock, const LvnMemoryBlockCreateInfo* createInfo)
{
    LVN_CMA_ASSERT(memBlock && createInfo, "memBlock and createInfo cannot be null");
    LVN_CMA_ASSERT(createInfo->align != 0 && (createInfo->align & (createInfo->align - 1)) == 0, "align cannot be zero or a non power of two");
    LVN_CMA_ASSERT(createInfo->align >= LVN_ALIGNOF(void*), "align must be >= default alignment");
    LVN_CMA_ASSERT(createInfo->size <= SIZE_MAX - createInfo->align - sizeof(LvnMemoryBlock), "size overflow, size + align + sizeof(LvnMemoryBlock) must be <= SIZE_MAX");

    int errResult = LVN_CMA_ERROR;
    LvnMemoryBlock* memBlockPtr = NULL;

    *memBlock = (LvnMemoryBlock*) LVN_CMA_MALLOC(sizeof(LvnMemoryBlock) + createInfo->size + createInfo->align);
    if (!*memBlock)
    {
        errResult = LVN_CMA_ALLOC_ERROR;
        goto fail_cleanup;
    }

    memset(*memBlock, 0, sizeof(LvnMemoryBlock) + createInfo->size + createInfo->align);
    memBlockPtr = *memBlock;

    memBlockPtr->allocation = (uint8_t*) memBlockPtr + sizeof(LvnMemoryBlock);
    memBlockPtr->allocation = (uint8_t*) LVN_ALIGN_UP((uintptr_t)memBlockPtr->allocation, createInfo->align);

    memBlockPtr->size = createInfo->size;
    memBlockPtr->currIndex = memBlockPtr->allocation;
    memBlockPtr->next = createInfo->next;

#ifdef LVN_CONFIG_DEBUG
    memset(memBlockPtr->allocation, LVN_DEBUG_FREE_VALUE, createInfo->size + createInfo->align);
#endif

    return LVN_CMA_SUCCESS;

fail_cleanup:
    if (*memBlock)
    {
        LVN_CMA_FREE(*memBlock);
        *memBlock = NULL;
    }
    return errResult;
}

void lvn_memBlockDestroy(LvnMemoryBlock* memBlock)
{
    if (!memBlock) { return; }
    LVN_CMA_FREE(memBlock);
}

void lvn_memBlockDestroyChain(LvnMemoryBlock* memBlock)
{
    while (memBlock)
    {
        LvnMemoryBlock* temp = memBlock;
        memBlock = memBlock->next;
        LVN_CMA_FREE(temp);
    }
}

size_t lvn_memBlockGetSize(LvnMemoryBlock* memBlock)
{
    LVN_CMA_ASSERT(memBlock, "memBlock cannot be null");
    return memBlock->size;
}

size_t lvn_memBlockGetOffset(LvnMemoryBlock* memBlock)
{
    LVN_CMA_ASSERT(memBlock, "memBlock cannot be null");
    return (uintptr_t)(memBlock->currIndex - memBlock->allocation);
}

int lvn_memPoolCreate(LvnMemoryPool* memPool, const LvnMemoryPoolCreateInfo* createInfo)
{
    LVN_CMA_ASSERT(memPool && createInfo, "memPool and createInfo cannot be null");
    LVN_CMA_ASSERT(createInfo->stride == 0 || createInfo->count <= SIZE_MAX / createInfo->stride, "overflow on creating memory size (count * stride)");
    LVN_CMA_ASSERT(createInfo->align != 0 && (createInfo->align & (createInfo->align - 1)) == 0, "align cannot be zero or a non power of two");
    LVN_CMA_ASSERT(createInfo->align >= LVN_ALIGNOF(void*), "align must be >= pointer alignment");

    int errResult = LVN_CMA_ERROR;
    LvnMemoryBlock* memBlock = NULL;

    // set min stride
    const size_t minStride = sizeof(void*);
    size_t stride = createInfo->stride < minStride ? minStride : createInfo->stride;

    // align stride
    size_t strideAligned = LVN_ALIGN_UP(stride, createInfo->align);

    LVN_CMA_ASSERT(strideAligned % createInfo->align == 0, "stride must be multiple of align");
    LVN_CMA_ASSERT(createInfo->count <= SIZE_MAX / strideAligned, "memory size overflow, count * strideAligned must be <= SIZE_MAX");

    // create memory block
    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = createInfo->count * strideAligned,
        .align = createInfo->align,
        .next = NULL,
    };

    int result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != LVN_CMA_SUCCESS)
    {
        errResult = result;
        goto fail_cleanup;
    }

    // fill memory pool info
    *memPool = (LvnMemoryPool){
        .front = memBlock,
        .back = memBlock,
        .freeList = NULL,
        .stride = createInfo->stride,
        .strideAligned = strideAligned,
        .align = createInfo->align,
        .flags = createInfo->flags,
        .allocCount = 0,
    };

    return LVN_CMA_SUCCESS;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

void lvn_memPoolDestroy(LvnMemoryPool* memPool)
{
    if (!memPool) { return; }

    lvn_memBlockDestroyChain(memPool->front);

    memPool->front = NULL;
    memPool->back = NULL;
    memPool->freeList = NULL;
    memPool->stride = 0;
    memPool->strideAligned = 0;
    memPool->align = 0;
    memPool->allocCount = 0;
}

int lvn_memPoolPushBlock(LvnMemoryPool* memPool, size_t count)
{
    LVN_CMA_ASSERT(memPool, "memPool cannot be null");
    LVN_CMA_ASSERT(count <= SIZE_MAX / memPool->strideAligned, "memory size overflow, count * strideAligned must be <= SIZE_MAX");

    if (!count)
        return LVN_CMA_SUCCESS;

    int errResult = LVN_CMA_ERROR;
    LvnMemoryBlock* memBlock = NULL;

    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = count * memPool->strideAligned,
        .align = memPool->align,
        .next = NULL,
    };

    int result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != LVN_CMA_SUCCESS)
    {
        errResult = result;
        goto fail_cleanup;
    }

    memPool->back->next = memBlock;
    memPool->back = memBlock;

    return LVN_CMA_SUCCESS;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

void* lvn_memPoolAlloc(LvnMemoryPool* memPool)
{
    LVN_CMA_ASSERT(memPool, "memPool cannot be null");

    void* ptr = NULL;

    // check free list
    if (memPool->freeList)
    {
        ptr = memPool->freeList;
        memPool->freeList = memPool->freeList->next;
        goto alloc_success;
    }

    // get next memory block index in pool if available
    for (LvnMemoryBlock* currBlock = memPool->front; currBlock; currBlock = currBlock->next)
    {
        if ((currBlock->currIndex + memPool->strideAligned) <= (currBlock->allocation + currBlock->size))
        {
            ptr = currBlock->currIndex;
            currBlock->currIndex += memPool->strideAligned;
            goto alloc_success;
        }
    }

    // check pool flags for dynamic growth
    if (!(memPool->flags & Lvn_MemoryPoolFlag_DynamicGrowth))
        return NULL;

    // create new memory block if no space left
    if (lvn_memPoolPushBlock(memPool, lvn_memPoolGetTotalCapacity(memPool)) != LVN_CMA_SUCCESS)
        return NULL;

    if ((memPool->back->currIndex + memPool->strideAligned) <= (memPool->back->allocation + memPool->back->size))
    {
        ptr = memPool->back->currIndex;
        memPool->back->currIndex += memPool->strideAligned;
        goto alloc_success;
    }

    // unable to find next block index
    return NULL;

alloc_success:
#ifdef LVN_CONFIG_DEBUG
    memset(ptr, LVN_DEBUG_ALLOC_VALUE, memPool->strideAligned);
#endif
    memPool->allocCount++;
    return ptr;
}

void lvn_memPoolFree(LvnMemoryPool* memPool, void* ptr)
{
    LVN_CMA_ASSERT(memPool, "memPool cannot be null");

    if (!ptr) { return; }

#ifdef LVN_CONFIG_DEBUG
    // find block the ptr was allocated from
    LvnMemoryBlock* currBlock = NULL;
    for (currBlock = memPool->front; currBlock; currBlock = currBlock->next)
    {
        if (lvn_ptrInBlock(currBlock->allocation, currBlock->size, ptr))
            break;
    }
    LVN_CMA_ASSERT(currBlock, "ptr not found within memory pool blocks");

    // check alignment of pointer
    size_t offset = (uint8_t*)ptr - (uint8_t*)currBlock->allocation;
    LVN_CMA_ASSERT(offset % memPool->strideAligned == 0, "invalid pool pointer, pointer not aligned to pool stride align");

    // checks if ptr was already freed (double free)
    for (LvnFreeNode* node = memPool->freeList; node; node = node->next)
    {
        LVN_CMA_ASSERT(node != ptr, "double free in memory pool");
    }
    memset(ptr, LVN_DEBUG_FREE_VALUE, memPool->strideAligned);
#endif

    // check if ptr is trying to be freed after pool reset/create
    LVN_CMA_ASSERT(memPool->allocCount > 0, "cannot free ptr to pool, pool was probably reset or just created");
    memPool->allocCount--;

    // free ptr, add to free list
    LvnFreeNode* node = (LvnFreeNode*) ptr;
    node->next = memPool->freeList;
    memPool->freeList = node;
}

void lvn_memPoolReset(LvnMemoryPool* memPool)
{
    LVN_CMA_ASSERT(memPool, "memPool cannot be null");

    for (LvnMemoryBlock* currBlock = memPool->front; currBlock; currBlock = currBlock->next)
    {
        currBlock->currIndex = currBlock->allocation;
#ifdef LVN_CONFIG_DEBUG
        memset(currBlock->allocation, LVN_DEBUG_FREE_VALUE, currBlock->size);
#endif
    }

    memPool->freeList = NULL;
    memPool->allocCount = 0;
}

int lvn_memPoolResetMergeBlocks(LvnMemoryPool* memPool)
{
    LVN_CMA_ASSERT(memPool, "memPool cannot be null");

    int errResult = LVN_CMA_ERROR;
    LvnMemoryBlock* memBlock = NULL;

    size_t totalSize = 0;
    for (LvnMemoryBlock* currBlock = memPool->front; currBlock; currBlock = currBlock->next)
        totalSize += currBlock->size;

    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = totalSize,
        .align = memPool->align,
        .next = NULL,
    };

    int result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != LVN_CMA_SUCCESS)
    {
        errResult = result;
        goto fail_cleanup;
    }

    lvn_memBlockDestroyChain(memPool->front);

    memPool->front = memBlock;
    memPool->back = memBlock;
    memPool->freeList = NULL;
    memPool->allocCount = 0;

    return LVN_CMA_SUCCESS;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

size_t lvn_memPoolGetTotalCapacity(LvnMemoryPool* memPool)
{
    LVN_CMA_ASSERT(memPool, "memPool cannot be null");

    size_t count = 0;

    for (LvnMemoryBlock* currBlock = memPool->front; currBlock; currBlock = currBlock->next)
        count += currBlock->size / memPool->strideAligned;

    return count;
}

size_t lvn_memPoolGetAllocCount(LvnMemoryPool* memPool)
{
    LVN_CMA_ASSERT(memPool, "memPool cannot be null");
    return memPool->allocCount;
}

int lvn_memArenaCreate(LvnMemoryArena* memArena, const LvnMemoryArenaCreateInfo* createInfo)
{
    LVN_CMA_ASSERT(memArena && createInfo, "memArena and createInfo cannot be null");
    LVN_CMA_ASSERT(createInfo->size <= SIZE_MAX, "arena size overflow");
    LVN_CMA_ASSERT(createInfo->align != 0 && (createInfo->align & (createInfo->align - 1)) == 0, "align cannot be zero or a non power of two");
    LVN_CMA_ASSERT(createInfo->align >= LVN_ALIGNOF(lvn_max_align_t), "align must be >= max align");

    int errResult = LVN_CMA_ERROR;
    LvnMemoryBlock* memBlock = NULL;

    // create memory block
    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = createInfo->size,
        .align = createInfo->align,
        .next = NULL,
    };

    int result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != LVN_CMA_SUCCESS)
    {
        errResult = result;
        goto fail_cleanup;
    }

    // fill memory arena info
    *memArena = (LvnMemoryArena){
        .front = memBlock,
        .back = memBlock,
        .align = createInfo->align,
        .flags = createInfo->flags,
        .generation = 0,
    };

    return LVN_CMA_SUCCESS;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

void lvn_memArenaDestroy(LvnMemoryArena* memArena)
{
    if (!memArena) { return; }

    lvn_memBlockDestroyChain(memArena->front);

    memArena->front = NULL;
    memArena->back = NULL;
    memArena->align = 0;
}

int lvn_memArenaPushBlock(LvnMemoryArena* memArena, size_t size)
{
    LVN_CMA_ASSERT(memArena, "memArena cannot be null");

    if (!size)
        return LVN_CMA_SUCCESS;

    int errResult = LVN_CMA_ERROR;
    LvnMemoryBlock* memBlock = NULL;

    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = size,
        .align = memArena->align,
        .next = NULL,
    };

    int result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != LVN_CMA_SUCCESS)
    {
        errResult = result;
        goto fail_cleanup;
    }

    memArena->back->next = memBlock;
    memArena->back = memBlock;

    return LVN_CMA_SUCCESS;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

void* lvn_memArenaAlloc(LvnMemoryArena* memArena, size_t size)
{
    LVN_CMA_ASSERT(memArena, "memArena cannot be null");
    return lvn_memArenaAllocAligned(memArena, size, memArena->align);
}

void* lvn_memArenaAllocAligned(LvnMemoryArena* memArena, size_t size, size_t align)
{
    LVN_CMA_ASSERT(memArena, "memArena cannot be null");
    LVN_CMA_ASSERT(align != 0 && (align & (align - 1)) == 0, "align cannot be zero or a non power of two");

    if (!size)
        return NULL;

    void* ptr = NULL;
    size_t newSize = 0;

    // get alloc from first block in arena if available
    uint8_t* alignedIndex = (uint8_t*) LVN_ALIGN_UP((uintptr_t)memArena->back->currIndex, align);
    if ((alignedIndex + size) <= (memArena->back->allocation + memArena->back->size))
    {
        ptr = alignedIndex;
        memArena->back->currIndex = alignedIndex + size;
        goto alloc_success;
    }

    // check pool flags for dynamic growth
    if (!(memArena->flags & Lvn_MemoryArenaFlag_DynamicGrowth))
        return NULL;

    // create new memory block if no space left
    LVN_CMA_ASSERT(memArena->back->size <= SIZE_MAX / 2, "arena size growth overflow");
    newSize = memArena->back->size * 2;
    newSize = (newSize < size) ? size : newSize;

    // check if align is greater than arena align, add to newSize if larger
    LVN_CMA_ASSERT(newSize <= SIZE_MAX - (align > memArena->align ? align : 0), "new size growth overflow on align");
    newSize += (align > memArena->align) ? align : 0;

    if (lvn_memArenaPushBlock(memArena, newSize) != LVN_CMA_SUCCESS)
        return NULL;

    alignedIndex = (uint8_t*) LVN_ALIGN_UP((uintptr_t)memArena->back->currIndex, align);
    if ((alignedIndex + size) <= (memArena->back->allocation + memArena->back->size))
    {
        ptr = alignedIndex;
        memArena->back->currIndex = alignedIndex + size;
        goto alloc_success;
    }

    // unable to find next block index
    return NULL;

alloc_success:
#ifdef LVN_CONFIG_DEBUG
    memset(ptr, LVN_DEBUG_ALLOC_VALUE, size);
#endif
    return ptr;
}

LvnArenaMark lvn_memArenaMark(LvnMemoryArena* memArena)
{
    LVN_CMA_ASSERT(memArena, "memArena cannot be null");
    return (LvnArenaMark){
        .block = memArena->back,
        .offset = (uintptr_t)(memArena->back->currIndex - memArena->back->allocation),
        .generation = memArena->generation,
    };
}

void lvn_memArenaMarkRevert(LvnMemoryArena* memArena, const LvnArenaMark* mark)
{
    LVN_CMA_ASSERT(memArena && mark, "memArena and mark cannot be null");
    LVN_CMA_ASSERT(mark->block, "mark block cannot be null");
    LVN_CMA_ASSERT(mark->offset <= mark->block->size, "mark offset must be <= block size");
    LVN_CMA_ASSERT(mark->offset <= (uintptr_t)(mark->block->currIndex - mark->block->allocation), "mark offset must be <= block current index");
    LVN_CMA_ASSERT(mark->generation == memArena->generation, "mark generation must be the same to memArena generation");

    LvnMemoryBlock* currBlock = NULL;

#ifdef LVN_CONFIG_DEBUG
    for (currBlock = memArena->front; currBlock; currBlock = currBlock->next)
    {
        if (currBlock == mark->block)
            break;
    }
    LVN_CMA_ASSERT(currBlock, "mark not found within memory arena");
#endif

    currBlock = mark->block->next;
    while (currBlock)
    {
        LvnMemoryBlock* temp = currBlock;
        currBlock = currBlock->next;
        lvn_memBlockDestroy(temp);
    }
    mark->block->next = NULL;

    memArena->back = mark->block;
    memArena->back->currIndex = memArena->back->allocation + mark->offset;

#ifdef LVN_CONFIG_DEBUG
    memset(memArena->back->allocation + mark->offset, LVN_DEBUG_FREE_VALUE, memArena->back->size - mark->offset);
#endif
}

int lvn_memArenaResetMergeBlocks(LvnMemoryArena* memArena)
{
    LVN_CMA_ASSERT(memArena, "memArena cannot be null");

    int errResult = LVN_CMA_ERROR;
    LvnMemoryBlock* memBlock = NULL;

    size_t totalSize = 0;
    for (LvnMemoryBlock* currBlock = memArena->front; currBlock; currBlock = currBlock->next)
        totalSize += currBlock->size;

    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = totalSize,
        .align = memArena->align,
        .next = NULL,
    };

    int result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != LVN_CMA_SUCCESS)
    {
        errResult = result;
        goto fail_cleanup;
    }

    lvn_memBlockDestroyChain(memArena->front);

    memArena->front = memBlock;
    memArena->back = memBlock;
    memArena->generation++;

    return LVN_CMA_SUCCESS;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

size_t lvn_memArenaGetTotalSize(LvnMemoryArena* memArena)
{
    LVN_CMA_ASSERT(memArena, "memArena cannot be null");

    size_t size = 0;

    for (LvnMemoryBlock* currBlock = memArena->front; currBlock; currBlock = currBlock->next)
        size += currBlock->size;

    return size;
}

LvnMemoryBlock* lvn_memArenaGetCurrBlock(LvnMemoryArena* memArena)
{
    LVN_CMA_ASSERT(memArena, "memArena cannot be null");

    return memArena->back;
}

#endif // LVN_CMA_IMPL


#endif // !LVN_CUSTOM_MEMORY_ALLOCATOR_H
