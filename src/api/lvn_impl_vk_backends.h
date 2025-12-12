#ifndef HG_LVN_VK_BACKENDS_H
#define HG_LVN_VK_BACKENDS_H


#include "lvn_graphics_internal.h"

#include <vulkan/vulkan.h>


typedef struct LvnVulkanBackends
{
    void* handle;
    PFN_vkGetInstanceProcAddr getInstanceProcAddr;

} LvnVulkanBackends;

#endif // !HG_LVN_VK_BACKENDS_H
