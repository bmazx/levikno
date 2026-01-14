#ifndef HG_LVN_VK_BACKENDS_H
#define HG_LVN_VK_BACKENDS_H


#include "lvn_graphics_internal.h"

#include <vulkan/vulkan.h>


typedef struct LvnVkQueueFamilyIndices
{
    uint32_t    graphicsIndex;
    uint32_t    presentIndex;
    bool        hasGraphics;
    bool        hasPresent;
} LvnVkQueueFamilyIndices;

typedef struct LvnVkSwapChainCreateInfo
{
    VkPhysicalDevice                  physicalDevice;
    VkSurfaceKHR                      surface;
    VkFormat                          surfaceFormat;
    VkPresentModeKHR                  presentMode;
    const LvnVkQueueFamilyIndices*    queueFamilyIndices;
    uint32_t                          width;
    uint32_t                          height;
    uint32_t                          minImageCount;
} LvnVkSwapChainCreateInfo;

typedef struct LvnVkSwapchainData
{
    VkSwapchainKHR      swapchain, oldSwapchain;
    VkExtent2D          swapchainExtent;
    VkFormat            swapchainFormat;
    VkPresentModeKHR    presentMode;
    uint32_t            swapchainImageCount;
    VkImage*            swapchainImages;
    VkImageView*        swapchainImageViews;
} LvnVkSwapchainData;

typedef struct LvnVulkanBackends
{
    void*                                            handle;
    PFN_vkGetInstanceProcAddr                        getInstanceProcAddr;
    PFN_vkEnumerateInstanceVersion                   enumerateInstanceVersion;
    PFN_vkEnumerateInstanceExtensionProperties       enumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties           enumerateInstanceLayerProperties;
    PFN_vkCreateInstance                             createInstance;
    PFN_vkDestroyInstance                            destroyInstance;
    PFN_vkCreateDebugUtilsMessengerEXT               createDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT              destroyDebugUtilsMessengerEXT;
    PFN_vkEnumeratePhysicalDevices                   enumeratePhysicalDevices;
    PFN_vkEnumerateDeviceExtensionProperties         enumerateDeviceExtensionProperties;
    PFN_vkGetPhysicalDeviceProperties                getPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceFormatProperties          getPhysicalDeviceFormatProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties     getPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR         getPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR    getPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR         getPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR    getPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkVoidFunction                               createSurfaceProc;
    PFN_vkDestroySurfaceKHR                          destroySurfaceKHR;
    PFN_vkGetDeviceProcAddr                          getDeviceProcAddr;
    PFN_vkCreateDevice                               createDevice;
    PFN_vkDestroyDevice                              destroyDevice;
    PFN_vkGetDeviceQueue                             getDeviceQueue;
    PFN_vkCreateSwapchainKHR                         createSwapchainKHR;
    PFN_vkDestroySwapchainKHR                        destroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                      getSwapchainImagesKHR;
    PFN_vkCreateImage                                createImage;
    PFN_vkDestroyImage                               destroyImage;
    PFN_vkCreateImageView                            createImageView;
    PFN_vkDestroyImageView                           destroyImageView;
    PFN_vkCreateShaderModule                         createShaderModule;
    PFN_vkDestroyShaderModule                        destroyShaderModule;
    PFN_vkCreateRenderPass                           createRenderPass;
    PFN_vkDestroyRenderPass                          destroyRenderPass;
    PFN_vkCreatePipelineLayout                       createPipelineLayout;
    PFN_vkDestroyPipelineLayout                      destroyPipelineLayout;
    PFN_vkCreateGraphicsPipelines                    createGraphicsPipelines;
    PFN_vkDestroyPipeline                            destroyPipeline;
    PFN_vkCreateFramebuffer                          createFramebuffer;
    PFN_vkDestroyFramebuffer                         destroyFramebuffer;
    PFN_vkCreateFence                                createFence;
    PFN_vkDestroyFence                               destroyFence;
    PFN_vkCreateSemaphore                            createSemaphore;
    PFN_vkDestroySemaphore                           destroySemaphore;
    PFN_vkCreateCommandPool                          createCommandPool;
    PFN_vkDestroyCommandPool                         destroyCommandPool;
    PFN_vkAllocateCommandBuffers                     allocateCommandBuffers;
    PFN_vkBeginCommandBuffer                         beginCommandBuffer;
    PFN_vkEndCommandBuffer                           endCommandBuffer;
    PFN_vkCmdBeginRendering                          cmdBeginRendering;
    PFN_vkCmdEndRendering                            cmdEndRendering;
    PFN_vkCmdBeginRenderPass                         cmdBeginRenderPass;
    PFN_vkCmdEndRenderPass                           cmdEndRenderPass;
    PFN_vkCmdPipelineBarrier                         cmdPipelineBarrier;
    PFN_vkCmdBindPipeline                            cmdBindPipeline;
    PFN_vkCmdSetViewport                             cmdSetViewport;
    PFN_vkCmdSetScissor                              cmdSetScissor;
    PFN_vkCmdDraw                                    cmdDraw;
    PFN_vkCmdDrawIndexed                             cmdDrawIndexed;
    PFN_vkAcquireNextImageKHR                        acquireNextImageKHR;
    PFN_vkQueueSubmit                                queueSubmit;
    PFN_vkQueuePresentKHR                            queuePresentKHR;
    PFN_vkWaitForFences                              waitForFences;
    PFN_vkResetFences                                resetFences;
    PFN_vkDeviceWaitIdle                             deviceWaitIdle;

    const LvnGraphicsContext*                        graphicsctx;
    uint32_t                                         versionMajor;
    uint32_t                                         versionMinor;
    bool                                             enableValidationLayers;
    VkInstance                                       instance;
    VkDebugUtilsMessengerEXT                         debugMessenger;
    VkPhysicalDevice                                 physicalDevice;
    LvnVkQueueFamilyIndices                          queueFamilyIndices;
    VkDevice                                         device;
    VkQueue                                          graphicsQueue;
    VkQueue                                          presentQueue;
    VkCommandPool                                    commandPool;

    struct
    {
        bool                                         KHR_surface;
        bool                                         KHR_win32_surface;
        bool                                         MVK_macos_surface;
        bool                                         EXT_metal_surface;
        bool                                         KHR_xlib_surface;
        bool                                         KHR_xcb_surface;
        bool                                         KHR_wayland_surface;
        bool                                         EXT_headless_surface;
    } ext;

} LvnVulkanBackends;

#endif // !HG_LVN_VK_BACKENDS_H
