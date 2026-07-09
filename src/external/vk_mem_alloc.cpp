#include "lvn_config.h"

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0

#ifdef LVN_CONFIG_DEBUG
    #define VMA_ASSERT(expr) (static_cast<bool>(expr) ? void(0) : LVN_ASSERT(0, "[VMA]: " #expr))
#endif

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
