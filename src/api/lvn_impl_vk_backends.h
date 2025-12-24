#ifndef HG_LVN_VK_BACKENDS_H
#define HG_LVN_VK_BACKENDS_H


#include "lvn_graphics_internal.h"

#include <vulkan/vulkan.h>


typedef struct LvnVkQueueFamilyIndices
{
    uint32_t graphicsIndex;
    uint32_t presentIndex;
    bool hasGraphics;
    bool hasPresent;
} LvnVkQueueFamilyIndices;

typedef struct LvnVkSwapChainCreateInfo
{
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    const LvnVkQueueFamilyIndices* queueFamilyIndices;
    uint32_t width;
    uint32_t height;
} LvnVkSwapChainCreateInfo;

typedef struct LvnVkSwapchainData
{
    VkSwapchainKHR swapchain;
    VkFormat swapchainFormat;
    VkExtent2D swapchainExtent;
    VkImage* swapchainImages;
    uint32_t swapchainImageCount;
} LvnVkSwapchainData;

typedef struct LvnVulkanBackends
{
    void*                                         handle;
    PFN_vkGetInstanceProcAddr                     getInstanceProcAddr;
    PFN_vkEnumerateInstanceExtensionProperties    enumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties        enumerateInstanceLayerProperties;
    PFN_vkCreateInstance                          createInstance;
    PFN_vkDestroyInstance                         destroyInstance;
    PFN_vkCreateDebugUtilsMessengerEXT            createDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT           destroyDebugUtilsMessengerEXT;
    PFN_vkEnumeratePhysicalDevices                enumeratePhysicalDevices;
    PFN_vkEnumerateDeviceExtensionProperties      enumerateDeviceExtensionProperties;
    PFN_vkGetPhysicalDeviceProperties             getPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties  getPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR      getPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR getPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR      getPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR getPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkVoidFunction                            createSurfaceProc;
    PFN_vkDestroySurfaceKHR                       destroySurfaceKHR;
    PFN_vkGetDeviceProcAddr                       getDeviceProcAddr;
    PFN_vkCreateDevice                            createDevice;
    PFN_vkDestroyDevice                           destroyDevice;
    PFN_vkGetDeviceQueue                          getDeviceQueue;
    PFN_vkCreateSwapchainKHR                      createSwapchainKHR;
    PFN_vkDestroySwapchainKHR                     destroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                   getSwapchainImagesKHR;

    const LvnGraphicsContext*                     graphicsctx;
    bool                                          enableValidationLayers;
    VkInstance                                    instance;
    VkDebugUtilsMessengerEXT                      debugMessenger;
    VkPhysicalDevice                              physicalDevice;
    VkDevice                                      device;
    VkQueue                                       graphicsQueue;
    VkQueue                                       presentQueue;

    struct
    {
        bool                                      KHR_surface;
        bool                                      KHR_win32_surface;
        bool                                      MVK_macos_surface;
        bool                                      EXT_metal_surface;
        bool                                      KHR_xlib_surface;
        bool                                      KHR_xcb_surface;
        bool                                      KHR_wayland_surface;
        bool                                      EXT_headless_surface;
    } ext;

} LvnVulkanBackends;

#endif // !HG_LVN_VK_BACKENDS_H
