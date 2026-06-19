#ifndef HG_LVN_VK_BACKENDS_H
#define HG_LVN_VK_BACKENDS_H


#include "lvn_graphics_internal.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

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
    VkSurfaceKHR        surface;
    VkSwapchainKHR      swapchain, oldSwapchain;
    VkExtent2D          swapchainExtent;
    VkFormat            swapchainFormat;
    VkPresentModeKHR    presentMode;
    uint32_t            swapchainImageCount;
    VkImage*            swapchainImages;
    VkImageView*        swapchainImageViews;
} LvnVkSwapchainData;

typedef struct LvnVkRenderpassData
{
    VkRenderPass    renderPass;
    bool            hasDepthStencil;
} LvnVkRenderpassData;

typedef struct LvnVkShaderData
{
    VkShaderModule        shaderModule;
    char*                 entryPoint;
    VkShaderStageFlags    shaderStage;
} LvnVkShaderData;

typedef struct LvnVkPipelineData
{
    VkPipeline          pipeline;
    VkPipelineLayout    pipelineLayout;
} LvnVkPipelineData;

typedef struct LvnVkBufferData
{
    VkBuffer              buffer;
    VmaAllocation         bufferMemory;
    void*                 bufferMap;
    VkBufferUsageFlags    usageFlags;
} LvnVkBufferData;

typedef struct LvnVkTextureData
{
    VkImage          image;
    VkImageView      imageView;
    VmaAllocation    imageMemory;
    VkSampler        sampler;
    uint32_t         width;
    uint32_t         height;
} LvnVkTextureData;

typedef struct LvnVulkanBackends
{
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
    VmaAllocator                                     vmaAllocator;
    LvnMemoryArena                                   frameArena;

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

    void*                                            handle;

    PFN_vkGetInstanceProcAddr                        vkGetInstanceProcAddr;
    PFN_vkEnumerateInstanceVersion                   vkEnumerateInstanceVersion;
    PFN_vkEnumerateInstanceExtensionProperties       vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties           vkEnumerateInstanceLayerProperties;
    PFN_vkCreateInstance                             vkCreateInstance;
    PFN_vkDestroyInstance                            vkDestroyInstance;
    PFN_vkCreateDebugUtilsMessengerEXT               vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT              vkDestroyDebugUtilsMessengerEXT;
    PFN_vkEnumeratePhysicalDevices                   vkEnumeratePhysicalDevices;
    PFN_vkEnumerateDeviceExtensionProperties         vkEnumerateDeviceExtensionProperties;
    PFN_vkGetPhysicalDeviceProperties                vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceFormatProperties          vkGetPhysicalDeviceFormatProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties     vkGetPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties          vkGetPhysicalDeviceMemoryProperties;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR         vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR    vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR         vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR    vkGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkGetPhysicalDeviceFeatures                  vkGetPhysicalDeviceFeatures;
    PFN_vkVoidFunction                               vkCreateSurfaceProc;
    PFN_vkDestroySurfaceKHR                          vkDestroySurfaceKHR;
    PFN_vkGetDeviceProcAddr                          vkGetDeviceProcAddr;
    PFN_vkCreateDevice                               vkCreateDevice;
    PFN_vkDestroyDevice                              vkDestroyDevice;
    PFN_vkGetDeviceQueue                             vkGetDeviceQueue;
    PFN_vkCreateSwapchainKHR                         vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR                        vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                      vkGetSwapchainImagesKHR;
    PFN_vkCreateImage                                vkCreateImage;
    PFN_vkDestroyImage                               vkDestroyImage;
    PFN_vkCreateImageView                            vkCreateImageView;
    PFN_vkDestroyImageView                           vkDestroyImageView;
    PFN_vkCreateSampler                              vkCreateSampler;
    PFN_vkDestroySampler                             vkDestroySampler;
    PFN_vkCreateShaderModule                         vkCreateShaderModule;
    PFN_vkDestroyShaderModule                        vkDestroyShaderModule;
    PFN_vkCreateRenderPass                           vkCreateRenderPass;
    PFN_vkDestroyRenderPass                          vkDestroyRenderPass;
    PFN_vkCreatePipelineLayout                       vkCreatePipelineLayout;
    PFN_vkDestroyPipelineLayout                      vkDestroyPipelineLayout;
    PFN_vkCreateGraphicsPipelines                    vkCreateGraphicsPipelines;
    PFN_vkDestroyPipeline                            vkDestroyPipeline;
    PFN_vkCreateFramebuffer                          vkCreateFramebuffer;
    PFN_vkDestroyFramebuffer                         vkDestroyFramebuffer;
    PFN_vkCreateBuffer                               vkCreateBuffer;
    PFN_vkDestroyBuffer                              vkDestroyBuffer;
    PFN_vkCreateFence                                vkCreateFence;
    PFN_vkDestroyFence                               vkDestroyFence;
    PFN_vkCreateSemaphore                            vkCreateSemaphore;
    PFN_vkDestroySemaphore                           vkDestroySemaphore;
    PFN_vkCreateCommandPool                          vkCreateCommandPool;
    PFN_vkDestroyCommandPool                         vkDestroyCommandPool;
    PFN_vkAllocateCommandBuffers                     vkAllocateCommandBuffers;
    PFN_vkFreeCommandBuffers                         vkFreeCommandBuffers;
    PFN_vkBeginCommandBuffer                         vkBeginCommandBuffer;
    PFN_vkEndCommandBuffer                           vkEndCommandBuffer;
    PFN_vkCmdBeginRenderPass                         vkCmdBeginRenderPass;
    PFN_vkCmdEndRenderPass                           vkCmdEndRenderPass;
    PFN_vkCmdPipelineBarrier                         vkCmdPipelineBarrier;
    PFN_vkCmdBindPipeline                            vkCmdBindPipeline;
    PFN_vkCmdBindVertexBuffers                       vkCmdBindVertexBuffers;
    PFN_vkCmdBindIndexBuffer                         vkCmdBindIndexBuffer;
    PFN_vkCmdSetViewport                             vkCmdSetViewport;
    PFN_vkCmdSetScissor                              vkCmdSetScissor;
    PFN_vkCmdDraw                                    vkCmdDraw;
    PFN_vkCmdDrawIndexed                             vkCmdDrawIndexed;
    PFN_vkCmdCopyBuffer                              vkCmdCopyBuffer;
    PFN_vkCmdCopyBufferToImage                       vkCmdCopyBufferToImage;
    PFN_vkAcquireNextImageKHR                        vkAcquireNextImageKHR;
    PFN_vkQueueSubmit                                vkQueueSubmit;
    PFN_vkQueuePresentKHR                            vkQueuePresentKHR;
    PFN_vkWaitForFences                              vkWaitForFences;
    PFN_vkResetFences                                vkResetFences;
    PFN_vkDeviceWaitIdle                             vkDeviceWaitIdle;
    PFN_vkQueueWaitIdle                              vkQueueWaitIdle;
    PFN_vkAllocateMemory                             vkAllocateMemory;
    PFN_vkFreeMemory                                 vkFreeMemory;
    PFN_vkMapMemory                                  vkMapMemory;
    PFN_vkUnmapMemory                                vkUnmapMemory;
    PFN_vkFlushMappedMemoryRanges                    vkFlushMappedMemoryRanges;
    PFN_vkInvalidateMappedMemoryRanges               vkInvalidateMappedMemoryRanges;
    PFN_vkBindBufferMemory                           vkBindBufferMemory;
    PFN_vkBindImageMemory                            vkBindImageMemory;
    PFN_vkGetBufferMemoryRequirements                vkGetBufferMemoryRequirements;
    PFN_vkGetImageMemoryRequirements                 vkGetImageMemoryRequirements;
} LvnVulkanBackends;

#endif // !HG_LVN_VK_BACKENDS_H
