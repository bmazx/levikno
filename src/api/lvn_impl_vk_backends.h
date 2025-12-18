#ifndef HG_LVN_VK_BACKENDS_H
#define HG_LVN_VK_BACKENDS_H


#include "lvn_graphics_internal.h"

#include <vulkan/vulkan.h>


typedef struct LvnVulkanBackends
{
    void* handle;
    PFN_vkGetInstanceProcAddr                    getInstanceProcAddr;
    PFN_vkEnumerateInstanceExtensionProperties   enumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties       enumerateInstanceLayerProperties;
    PFN_vkCreateInstance                         createInstance;
    PFN_vkDestroyInstance                        destroyInstance;
    PFN_vkCreateDebugUtilsMessengerEXT           createDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT          destroyDebugUtilsMessengerEXT;
    PFN_vkEnumeratePhysicalDevices               enumeratePhysicalDevices;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties getPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR     getPhysicalDeviceSurfaceSupportKHR;

    VkInstance                                   instance;
    VkDebugUtilsMessengerEXT                     debugMessenger;
    VkPhysicalDevice                             physicalDevice;

    struct
    {
        bool                                     KHR_surface;
        bool                                     KHR_win32_surface;
        bool                                     MVK_macos_surface;
        bool                                     EXT_metal_surface;
        bool                                     KHR_xlib_surface;
        bool                                     KHR_xcb_surface;
        bool                                     KHR_wayland_surface;
        bool                                     EXT_headless_surface;
    } ext;

} LvnVulkanBackends;

#endif // !HG_LVN_VK_BACKENDS_H
