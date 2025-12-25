#include "lvn_impl_vk.h"
#include "lvn_impl_vk_backends.h"

#include <string.h>


#if defined(LVN_INCLUDE_WAYLAND)
    #include <vulkan/vulkan_wayland.h>
#endif

#if defined(LVN_PLATFORM_LINUX)
    static const char* s_LvnVkLibName = "libvulkan.so.1";
#elif defined(LVN_PLATFORM_WINDOWS)
    static const char* s_LvnVkLibName = "vulkan-1.dll";
#elif defined(LVN_PLATFORM_MACOS)
    static const char* s_LvnVkLibName = "libvulkan.1.dylib";
#endif

static const char* s_LvnVkValidationLayers[] =
{
    "VK_LAYER_KHRONOS_validation",
};

static LvnResult                   lvn_createPlatformSurface(const LvnVulkanBackends* vkBackends, VkSurfaceKHR* surface, const LvnPlatformData* platformData);
static LvnVkQueueFamilyIndices     lvn_findQueueFamilies(const LvnVulkanBackends* vkBackends, VkPhysicalDevice device, VkSurfaceKHR surface);
static bool                        lvn_checkDeviceExtensionSupport(const LvnVulkanBackends* vkBackends, VkPhysicalDevice device, const char** requiredExtensions, uint32_t requiredExtensionCount);
static VkPhysicalDevice            lvn_getBestPhysicalDevice(const LvnVulkanBackends* vkBackends, VkSurfaceKHR surface);
static LvnResult                   lvn_createSwapChain(const LvnVulkanBackends* vkBackends, LvnVkSwapchainData* swapchainData, const LvnVkSwapChainCreateInfo* createInfo);

static VKAPI_ATTR VkBool32 VKAPI_CALL lvn_debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    LvnGraphicsContext* graphicsctx = (LvnGraphicsContext*) pUserData;

    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        {
            LVN_LOG_INFO(graphicsctx->coreLogger, "vulkan validation Layer: %s", pCallbackData->pMessage);
            return VK_TRUE;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        {
            LVN_LOG_WARN(graphicsctx->coreLogger, "vulkan validation Layer: %s", pCallbackData->pMessage);
            return VK_TRUE;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger, "vulkan validation Layer: %s", pCallbackData->pMessage);
            return VK_TRUE;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger, "vulkan validation Layer: %s", pCallbackData->pMessage);
            return VK_TRUE;
        }
        default:
        {
            return VK_FALSE;
        }
    }

    return VK_FALSE;
}

static LvnResult lvn_createPlatformSurface(const LvnVulkanBackends* vkBackends, VkSurfaceKHR* surface, const LvnPlatformData* platformData)
{
    VkResult result;

#if defined(LVN_INCLUDE_WAYLAND)
    VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {0};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.display = (struct wl_display*) platformData->nativeDisplayHandle;
    surfaceCreateInfo.surface = (struct wl_surface*) platformData->nativeWindowHandle;
    PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR_PFN =
        (PFN_vkCreateWaylandSurfaceKHR) vkBackends->createSurfaceProc;
    result = vkCreateWaylandSurfaceKHR_PFN(vkBackends->instance, &surfaceCreateInfo, NULL, surface);
#endif

    return result == VK_SUCCESS ? Lvn_Result_Success : Lvn_Result_Failure;
}

static LvnVkQueueFamilyIndices lvn_findQueueFamilies(const LvnVulkanBackends* vkBackends, VkPhysicalDevice device, VkSurfaceKHR surface)
{
    LvnVkQueueFamilyIndices indices = {0};

    VkQueueFamilyProperties* queueFamilies = NULL;
    uint32_t queueFamilyCount = 0;

    vkBackends->getPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    queueFamilies = lvn_calloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkBackends->getPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsIndex = i;
            indices.hasGraphics = true;
        }

        if (surface != NULL)
        {
            VkBool32 presentSupport = VK_FALSE;
            vkBackends->getPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport == VK_TRUE)
            {
                indices.presentIndex = i;
                indices.hasPresent = true;
            }
        }

        // return only graphics index if no surface is provided; present support discluded
        if (indices.hasGraphics && (!surface || indices.hasPresent))
            break;
    }

    lvn_free(queueFamilies);

    return indices;
}

static bool lvn_checkDeviceExtensionSupport(
    const LvnVulkanBackends* vkBackends,
    VkPhysicalDevice physicalDevice,
    const char** requiredExtensions,
    uint32_t requiredExtensionCount)
{
    LVN_ASSERT(vkBackends && physicalDevice, "vkBackends and physicalDevice cannot be null");

    if (!requiredExtensions || !requiredExtensionCount)
        return true;

    VkExtensionProperties* extensions = NULL;
    uint32_t extensionCount = 0;

    vkBackends->enumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL);
    extensions = lvn_calloc(extensionCount * sizeof(VkExtensionProperties));
    vkBackends->enumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, extensions);

    for (uint32_t i = 0; i < requiredExtensionCount; i++)
    {
        bool extensionFound = false;
        for (uint32_t j = 0; j < extensionCount; j++)
        {
            if (!strcmp(requiredExtensions[i], extensions[j].extensionName))
            {
                extensionFound = true;
                break;
            }
        }

        if (!extensionFound)
        {
            LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to find required device extension: %s", requiredExtensions[i]);
            goto fail_cleanup;
        }
    }

    lvn_free(extensions);
    return true;

fail_cleanup:
    lvn_free(extensions);
    return false;
}

static VkPhysicalDevice lvn_getBestPhysicalDevice(const LvnVulkanBackends* vkBackends, VkSurfaceKHR surface)
{
    LVN_ASSERT(vkBackends, "vkBackends cannot be null");

    VkPhysicalDevice* physicalDevices = NULL;
    uint32_t physicalDeviceCount = 0;

    vkBackends->enumeratePhysicalDevices(vkBackends->instance, &physicalDeviceCount, NULL);
    physicalDevices = lvn_calloc(physicalDeviceCount * sizeof(VkPhysicalDevice));
    vkBackends->enumeratePhysicalDevices(vkBackends->instance, &physicalDeviceCount, physicalDevices);

    const char* requiredExtensions = NULL;
    uint32_t requiredExtensionCount = 0;

    // get device extensions for surface present support
    if (surface)
    {
        requiredExtensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        requiredExtensionCount = 1;
    }

    uint32_t bestScore = 0;
    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

    for (uint32_t i = 0; i < physicalDeviceCount; i++)
    {
        VkPhysicalDevice physicalDevice = physicalDevices[i];

        const LvnVkQueueFamilyIndices queueIndices = lvn_findQueueFamilies(vkBackends, physicalDevice, surface);

        // check queue families
        if (!queueIndices.hasGraphics || (surface && !queueIndices.hasPresent))
            continue;

        // check device extension support
        if (!lvn_checkDeviceExtensionSupport(vkBackends, physicalDevice, &requiredExtensions, requiredExtensionCount))
            continue;

        VkPhysicalDeviceProperties deviceProperties;
        vkBackends->getPhysicalDeviceProperties(physicalDevice, &deviceProperties);

        size_t score = 0;

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;
        else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            score += 500;
        else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
            score += 100;
        else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
            score += 10;
        else
            score += 1;

        if (score > bestScore)
        {
            bestScore = score;
            bestDevice = physicalDevice;
        }
    }

    lvn_free(physicalDevices);

    return bestDevice;
}

static LvnResult lvn_createSwapChain(const LvnVulkanBackends* vkBackends, LvnVkSwapchainData* swapchainData, const LvnVkSwapChainCreateInfo* createInfo)
{
    LVN_ASSERT(vkBackends && swapchainData && createInfo, "vkBackends, swapchain, and createInfo cannot be null");
    LVN_ASSERT(createInfo->surface && createInfo->physicalDevice && createInfo->queueFamilyIndices, "createInfo->surface, createInfo->physicalDevice, and createInfo->queueFamilyIndices cannot be null");

    VkSurfaceFormatKHR* surfaceFormats = NULL;
    VkPresentModeKHR* presentModes = NULL;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkImage* swapchainImages = NULL;
    VkImageView* swapchainImageViews = NULL;

    // check for swapchain capabilitie support
    VkSurfaceCapabilitiesKHR capabilities;
    vkBackends->getPhysicalDeviceSurfaceCapabilitiesKHR(createInfo->physicalDevice, createInfo->surface, &capabilities);

    // swapchain format
    uint32_t formatCount;
    vkBackends->getPhysicalDeviceSurfaceFormatsKHR(createInfo->physicalDevice, createInfo->surface, &formatCount, NULL);

    if (!formatCount)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to create swapchain, no supported surface formats found");
        goto fail_cleanup;
    }

    surfaceFormats = lvn_calloc(formatCount * sizeof(VkSurfaceFormatKHR));
    vkBackends->getPhysicalDeviceSurfaceFormatsKHR(createInfo->physicalDevice, createInfo->surface, &formatCount, surfaceFormats);

    // swapchain present modes
    uint32_t presentModeCount;
    vkBackends->getPhysicalDeviceSurfacePresentModesKHR(createInfo->physicalDevice, createInfo->surface, &presentModeCount, NULL);

    if (!presentModeCount)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to create swapchain, no supported present mode found");
        goto fail_cleanup;
    }

    presentModes = lvn_calloc(presentModeCount * sizeof(VkPresentModeKHR));
    vkBackends->getPhysicalDeviceSurfacePresentModesKHR(createInfo->physicalDevice, createInfo->surface, &presentModeCount, presentModes);

    // find desired format, default to first if not found
    VkSurfaceFormatKHR surfaceFormat = surfaceFormats[0];
    for (uint32_t i = 0; i < formatCount; i++)
    {
        if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = surfaceFormats[i];
            break;
        }
    }

    // find desired present mode
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < presentModeCount; i++)
    {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = presentModes[i];
            break;
        }
    }

    // choose swapchain extent
    VkExtent2D extent = capabilities.currentExtent;
    if (capabilities.currentExtent.width == UINT32_MAX)
    {
        // clamp extent values between min and max image extent
        extent.width = (createInfo->width > capabilities.maxImageExtent.width)
            ? capabilities.maxImageExtent.width
            : (createInfo->width < capabilities.minImageExtent.width)
            ? capabilities.minImageExtent.width
            : createInfo->width;
        extent.height = (createInfo->height > capabilities.maxImageExtent.height)
            ? capabilities.maxImageExtent.height
            : (createInfo->height < capabilities.minImageExtent.height)
            ? capabilities.minImageExtent.height
            : createInfo->height;
    }

    // get image count, set 3 as default for triple buffering
    uint32_t imageCount = 3;

    // if no max image count get the highest image count required
    if (capabilities.maxImageCount == 0)
        imageCount = (imageCount < capabilities.minImageCount) ? capabilities.minImageCount : imageCount;
    else
    {
        // clamp image count between min and max image count
        imageCount = (imageCount > capabilities.maxImageCount)
            ? capabilities.maxImageCount
            : (imageCount < capabilities.minImageCount)
            ? capabilities.minImageCount
            : imageCount;

    }

    // create swapchain
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {0};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = createInfo->surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform = capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    uint32_t queueFamilyIndices[] = { createInfo->queueFamilyIndices->graphicsIndex, createInfo->queueFamilyIndices->presentIndex };
    if (createInfo->queueFamilyIndices->graphicsIndex != createInfo->queueFamilyIndices->presentIndex)
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = NULL;
    }

    if (vkBackends->createSwapchainKHR(vkBackends->device, &swapchainCreateInfo, NULL, &swapchain) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to create swapchain");
        goto fail_cleanup;
    }

    // get swapchain images
    uint32_t swapchainImageCount;
    vkBackends->getSwapchainImagesKHR(vkBackends->device, swapchain, &swapchainImageCount, NULL);
    swapchainImages = lvn_calloc(swapchainImageCount * sizeof(VkImage));
    vkBackends->getSwapchainImagesKHR(vkBackends->device, swapchain, &swapchainImageCount, swapchainImages);

    // get swapchain image views
    swapchainImageViews = lvn_calloc(swapchainImageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {0};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = surfaceFormat.format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        if (vkBackends->createImageView(vkBackends->device, &imageViewCreateInfo, NULL, &swapchainImageViews[i]) != VK_SUCCESS)
        {
            LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to create swapchain image views");
            goto fail_cleanup;
        }
    }

    swapchainData->swapchain = swapchain;
    swapchainData->swapchainFormat = surfaceFormat.format;
    swapchainData->swapchainExtent = extent;
    swapchainData->swapchainImages = swapchainImages;
    swapchainData->swapchainImageCount = swapchainImageCount;
    swapchainData->swapchainImageViews = swapchainImageViews;

    lvn_free(surfaceFormats);
    lvn_free(presentModes);

    return Lvn_Result_Success;

fail_cleanup:
    vkBackends->destroySwapchainKHR(vkBackends->device, swapchain, NULL);
    lvn_free(surfaceFormats);
    lvn_free(presentModes);
    lvn_free(swapchainImages);
    for (uint32_t i = 0; i < swapchainImageCount; i++)
        vkBackends->destroyImageView(vkBackends->device, swapchainImageViews[i], NULL);
    lvn_free(swapchainImageViews);
    return Lvn_Result_Failure;
}

LvnResult lvnImplVkInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && createInfo, "graphicsctx and createInfo cannot be nullptr");

    const char** extensionNames = NULL;
    VkExtensionProperties* extensionProps = NULL;
    VkLayerProperties* availableLayers = NULL;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    LvnVulkanBackends* vkBackends = lvn_calloc(sizeof(LvnVulkanBackends));
    graphicsctx->implData = vkBackends;

    vkBackends->graphicsctx = graphicsctx;
    vkBackends->enableValidationLayers = createInfo->enableGraphicsApiDebugLogging;

    // load vulkan library
    vkBackends->handle = lvn_platformLoadModule(s_LvnVkLibName);

    if (!vkBackends->handle)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] unable to load vulkan shared library: %s",
                      s_LvnVkLibName);
        goto fail_cleanup;
    }

    vkBackends->getInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
        lvn_platformGetModuleSymbol(vkBackends->handle, "vkGetInstanceProcAddr");

    if (!vkBackends->getInstanceProcAddr)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] unable to retrieve vkGetInstanceProcAddr symbol");
        goto fail_cleanup;
    }

    vkBackends->enumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)
        vkBackends->getInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
    vkBackends->enumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)
        vkBackends->getInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");
    vkBackends->createInstance = (PFN_vkCreateInstance)
        vkBackends->getInstanceProcAddr(NULL, "vkCreateInstance");


    if (!vkBackends->enumerateInstanceExtensionProperties ||
        !vkBackends->enumerateInstanceLayerProperties ||
        !vkBackends->createInstance)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to load vulkan global level function symbols");
        goto fail_cleanup;
    }


    // query vulkan instance exensions for surface support
    uint32_t extensionCount = 0;
    if (createInfo->presentationModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        uint32_t extensionPropsCount;
        VkResult result = vkBackends->enumerateInstanceExtensionProperties(NULL, &extensionPropsCount, NULL);
        if (result != VK_SUCCESS)
        {
            // NOTE: this happens on systems with a loader but without any vulkan ICD
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to query vulkan instance extensions");
            goto fail_cleanup;
        }

        extensionProps = lvn_calloc(extensionPropsCount * sizeof(VkExtensionProperties));
        result = vkBackends->enumerateInstanceExtensionProperties(NULL, &extensionPropsCount, extensionProps);
        if (result != VK_SUCCESS)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to query vulkan instance extensions");
            goto fail_cleanup;
        }

        for (uint32_t i = 0; i < extensionPropsCount; i++)
        {
            if (strcmp(extensionProps[i].extensionName, "VK_KHR_surface") == 0)
                vkBackends->ext.KHR_surface = true;
            else if (strcmp(extensionProps[i].extensionName, "VK_KHR_win32_surface") == 0)
                vkBackends->ext.KHR_win32_surface = true;
            else if (strcmp(extensionProps[i].extensionName, "VK_MVK_macos_surface") == 0)
                vkBackends->ext.MVK_macos_surface = true;
            else if (strcmp(extensionProps[i].extensionName, "VK_EXT_metal_surface") == 0)
                vkBackends->ext.EXT_metal_surface = true;
            else if (strcmp(extensionProps[i].extensionName, "VK_KHR_xlib_surface") == 0)
                vkBackends->ext.KHR_xlib_surface = true;
            else if (strcmp(extensionProps[i].extensionName, "VK_KHR_xcb_surface") == 0)
                vkBackends->ext.KHR_xcb_surface = true;
            else if (strcmp(extensionProps[i].extensionName, "VK_KHR_wayland_surface") == 0)
                vkBackends->ext.KHR_wayland_surface = true;
            else if (strcmp(extensionProps[i].extensionName, "VK_EXT_headless_surface") == 0)
                vkBackends->ext.EXT_headless_surface = true;
        }

        if (vkBackends->ext.KHR_surface)
        {
            extensionNames = lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_surface";
        }
        if (vkBackends->ext.KHR_win32_surface)
        {
            extensionNames = lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_win32_surface";
        }
        if (vkBackends->ext.MVK_macos_surface)
        {
            extensionNames = lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_MVK_macos_surface";
        }
        if (vkBackends->ext.EXT_metal_surface)
        {
            extensionNames = lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_EXT_metal_surface";
        }
        if (vkBackends->ext.KHR_xlib_surface)
        {
            extensionNames = lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_xlib_surface";
        }
        if (vkBackends->ext.KHR_xcb_surface)
        {
            extensionNames = lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_xcb_surface";
        }
        if (vkBackends->ext.KHR_wayland_surface)
        {
            extensionNames = lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_wayland_surface";
        }
        if (vkBackends->ext.EXT_headless_surface)
        {
            extensionNames = lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_EXT_headless_surface";
        }
    }

    // check validation layer support
    bool layerSupport = true;
    uint32_t availableLayerCount = 0;
    if (vkBackends->enableValidationLayers)
    {
        vkBackends->enumerateInstanceLayerProperties(&availableLayerCount, NULL);
        availableLayers = lvn_calloc(availableLayerCount * sizeof(VkLayerProperties));
        vkBackends->enumerateInstanceLayerProperties(&availableLayerCount, availableLayers);

        for (uint32_t i = 0; i < LVN_ARRAY_LEN(s_LvnVkValidationLayers); i++)
        {
            bool layerFound = false;
            for (uint32_t j = 0; j < availableLayerCount; j++)
            {
                if (strcmp(availableLayers[j].layerName, s_LvnVkValidationLayers[i]) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                layerSupport = false;
                break;
            }
        }

        // add validation message callback extension
        if (layerSupport)
        {
            extensionNames = lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        }

    }

    // create debug message util
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = lvn_debugCallback;
    debugCreateInfo.pUserData = graphicsctx;

    // create vulkan instance
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = graphicsctx->ctx->appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = NULL;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo vkCreateInfo = {0};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkCreateInfo.pApplicationInfo = &appInfo;
    vkCreateInfo.enabledExtensionCount = extensionCount;
    vkCreateInfo.ppEnabledExtensionNames = extensionNames;

    if (vkBackends->enableValidationLayers)
    {
        if (layerSupport)
        {
            vkCreateInfo.enabledLayerCount = LVN_ARRAY_LEN(s_LvnVkValidationLayers);
            vkCreateInfo.ppEnabledLayerNames = s_LvnVkValidationLayers;
            vkCreateInfo.pNext = &debugCreateInfo;
        }
        else
        {
            LVN_LOG_WARN(graphicsctx->coreLogger,
                         "[vulkan] validation layers unsupported, skipping debug logging support");
            vkCreateInfo.enabledLayerCount = 0;
            vkCreateInfo.ppEnabledLayerNames = NULL;
            vkCreateInfo.pNext = NULL;
        }
    }
    else
    {
        vkCreateInfo.enabledLayerCount = 0;
        vkCreateInfo.ppEnabledLayerNames = NULL;
        vkCreateInfo.pNext = NULL;
    }

    if (vkBackends->createInstance(&vkCreateInfo, NULL, &vkBackends->instance) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create instance");
        goto fail_cleanup;
    }

    // get instance level function symbols
    vkBackends->destroyInstance = (PFN_vkDestroyInstance)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkDestroyInstance");
    vkBackends->enumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkEnumeratePhysicalDevices");
    vkBackends->getPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    vkBackends->enumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkEnumerateDeviceExtensionProperties");
    vkBackends->getPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceProperties");
    vkBackends->getDeviceProcAddr = (PFN_vkGetDeviceProcAddr)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetDeviceProcAddr");
    vkBackends->createDevice = (PFN_vkCreateDevice)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkCreateDevice");

    if (!vkBackends->destroyInstance ||
        !vkBackends->enumeratePhysicalDevices ||
        !vkBackends->getPhysicalDeviceQueueFamilyProperties ||
        !vkBackends->enumerateDeviceExtensionProperties ||
        !vkBackends->getPhysicalDeviceProperties ||
        !vkBackends->getDeviceProcAddr ||
        !vkBackends->createDevice)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to load vulkan instance level function symbols");
        goto fail_cleanup;
    }

    if (createInfo->presentationModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        vkBackends->getPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
            vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        vkBackends->getPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
            vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        vkBackends->getPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)
            vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        vkBackends->getPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)
            vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
        vkBackends->destroySurfaceKHR = (PFN_vkDestroySurfaceKHR)
            vkBackends->getInstanceProcAddr(vkBackends->instance, "vkDestroySurfaceKHR");


        // get create surface PFN based on window platform
#if defined(LVN_INCLUDE_WAYLAND)
        vkBackends->createSurfaceProc = (PFN_vkVoidFunction)
            vkBackends->getInstanceProcAddr(vkBackends->instance, "vkCreateWaylandSurfaceKHR");
#endif

        if (!vkBackends->getPhysicalDeviceSurfaceSupportKHR ||
            !vkBackends->getPhysicalDeviceSurfaceCapabilitiesKHR ||
            !vkBackends->getPhysicalDeviceSurfaceFormatsKHR ||
            !vkBackends->getPhysicalDeviceSurfacePresentModesKHR ||
            !vkBackends->destroySurfaceKHR ||
            !vkBackends->createSurfaceProc)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to load vulkan instance level surface function symbol");
            goto fail_cleanup;
        }
    }

    // create debug messegenger if debug logging enabled
    if (vkBackends->enableValidationLayers)
    {
        vkBackends->createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
            vkBackends->getInstanceProcAddr(vkBackends->instance, "vkCreateDebugUtilsMessengerEXT");
        vkBackends->destroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkBackends->getInstanceProcAddr(vkBackends->instance, "vkDestroyDebugUtilsMessengerEXT");

        if (!vkBackends->createDebugUtilsMessengerEXT ||
            !vkBackends->destroyDebugUtilsMessengerEXT)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to load vulkan debug message function symbols");
            goto fail_cleanup;
        }

        if (vkBackends->createDebugUtilsMessengerEXT(vkBackends->instance, &debugCreateInfo, NULL, &vkBackends->debugMessenger) != VK_SUCCESS)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to create debug message utils");
            goto fail_cleanup;
        }
    }

    // create surface
    if (graphicsctx->presentModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        if (lvn_createPlatformSurface(vkBackends, &surface, createInfo->platformData) != Lvn_Result_Success)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to create temporary surface during vulkan init");
            goto fail_cleanup;
        }
    }

    // get default physical device without surface support
    vkBackends->physicalDevice = lvn_getBestPhysicalDevice(vkBackends, surface);

    if (vkBackends->physicalDevice == VK_NULL_HANDLE)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to find suitable physical device");
        goto fail_cleanup;
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkBackends->getPhysicalDeviceProperties(vkBackends->physicalDevice, &deviceProperties);

    LVN_LOG_TRACE(graphicsctx->coreLogger,
                  "[vulkan] found supported physical device: \"%s\", driverVersion: (%u), apiVersion: (%u)",
                  deviceProperties.deviceName,
                  deviceProperties.driverVersion,
                  deviceProperties.apiVersion);

    // create logical device
    LvnVkQueueFamilyIndices indices = lvn_findQueueFamilies(vkBackends, vkBackends->physicalDevice, surface);
    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {0};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.enabledExtensionCount = 0;

    if (vkBackends->enableValidationLayers)
    {
        deviceCreateInfo.enabledLayerCount = LVN_ARRAY_LEN(s_LvnVkValidationLayers);
        deviceCreateInfo.ppEnabledLayerNames = s_LvnVkValidationLayers;
    }

    const char* requiredExtensions = NULL;
    uint32_t requiredExtensionCount = 0;

    if (graphicsctx->presentModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        requiredExtensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        requiredExtensionCount = 1;
        deviceCreateInfo.ppEnabledExtensionNames = &requiredExtensions;
        deviceCreateInfo.enabledExtensionCount = requiredExtensionCount;
    }

    if (vkBackends->createDevice(vkBackends->physicalDevice, &deviceCreateInfo, NULL, &vkBackends->device) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create logical device");
        goto fail_cleanup;
    }

    // get device level function symbols
    vkBackends->destroyDevice = (PFN_vkDestroyDevice)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyDevice");
    vkBackends->getDeviceQueue = (PFN_vkGetDeviceQueue)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkGetDeviceQueue");
    vkBackends->createImage = (PFN_vkCreateImage)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateImage");
    vkBackends->destroyImage = (PFN_vkDestroyImage)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyImage");
    vkBackends->createImageView = (PFN_vkCreateImageView)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateImageView");
    vkBackends->destroyImageView = (PFN_vkDestroyImageView)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyImageView");
    vkBackends->createShaderModule = (PFN_vkCreateShaderModule)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateShaderModule");
    vkBackends->destroyShaderModule = (PFN_vkDestroyShaderModule)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyShaderModule");

    if (!vkBackends->destroyDevice ||
        !vkBackends->getDeviceQueue ||
        !vkBackends->createImage ||
        !vkBackends->destroyImage ||
        !vkBackends->createImageView ||
        !vkBackends->destroyImageView ||
        !vkBackends->createShaderModule ||
        !vkBackends->destroyShaderModule)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to load vulkan device level function symbols");
        goto fail_cleanup;
    }

    if (graphicsctx->presentModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        vkBackends->createSwapchainKHR = (PFN_vkCreateSwapchainKHR)
            vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateSwapchainKHR");
        vkBackends->destroySwapchainKHR = (PFN_vkDestroySwapchainKHR)
            vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroySwapchainKHR");
        vkBackends->getSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)
            vkBackends->getDeviceProcAddr(vkBackends->device, "vkGetSwapchainImagesKHR");

        if (!vkBackends->createSwapchainKHR ||
            !vkBackends->destroySwapchainKHR ||
            !vkBackends->getSwapchainImagesKHR)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to load vulkan device level surface function symbol");
            goto fail_cleanup;
        }

    }

    // get graphics and present queues from device
    vkBackends->getDeviceQueue(vkBackends->device, indices.graphicsIndex, 0, &vkBackends->graphicsQueue);

    if (graphicsctx->presentModeFlags & Lvn_PresentationModeFlag_Surface)
        vkBackends->getDeviceQueue(vkBackends->device, indices.presentIndex, 0, &vkBackends->presentQueue);


    // set vulkan implementation function pointers
    graphicsctx->implCreateSurface = lvnImplVkCreateSurface;
    graphicsctx->implDestroySurface = lvnImplVkDestroySurface;

    if (surface) vkBackends->destroySurfaceKHR(vkBackends->instance, surface, NULL);
    lvn_free(extensionProps);
    lvn_free(extensionNames);
    lvn_free(availableLayers);

    return Lvn_Result_Success;

fail_cleanup:
    if (surface) vkBackends->destroySurfaceKHR(vkBackends->instance, surface, NULL);
    lvn_free(extensionProps);
    lvn_free(extensionNames);
    lvn_free(availableLayers);
    lvnImplVkTerminate(graphicsctx);
    return Lvn_Result_Failure;
}

void lvnImplVkTerminate(LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    LvnVulkanBackends* vkBackends = (LvnVulkanBackends*) graphicsctx->implData;

    if (vkBackends->device)
        vkBackends->destroyDevice(vkBackends->device, NULL);
    if (vkBackends->debugMessenger)
        vkBackends->destroyDebugUtilsMessengerEXT(vkBackends->instance, vkBackends->debugMessenger, NULL);
    if (vkBackends->instance)
        vkBackends->destroyInstance(vkBackends->instance, NULL);

    if (vkBackends->handle)
        lvn_platformFreeModule(vkBackends->handle);

    lvn_free(vkBackends);
    graphicsctx->implData = NULL;
}

LvnResult lvnImplVkCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && surface && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
    LvnVkSwapchainData* swapchainData = NULL;

    LvnPlatformData platformData = {0};
    platformData.nativeDisplayHandle = createInfo->nativeDisplayHandle;
    platformData.nativeWindowHandle = createInfo->nativeWindowHandle;

    if (lvn_createPlatformSurface(vkBackends, &vkSurface, &platformData) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create VkSurfaceKHR for surface %p", surface);
        goto fail_cleanup;
    }

    LvnVkQueueFamilyIndices queueFamilyIndices = lvn_findQueueFamilies(vkBackends, vkBackends->physicalDevice, vkSurface);

    LvnVkSwapChainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.physicalDevice = vkBackends->physicalDevice;
    swapchainCreateInfo.surface = vkSurface;
    swapchainCreateInfo.queueFamilyIndices = &queueFamilyIndices;
    swapchainCreateInfo.width = createInfo->width;
    swapchainCreateInfo.height = createInfo->height;

    swapchainData = lvn_calloc(sizeof(LvnVkSwapchainData));
    if (lvn_createSwapChain(vkBackends, swapchainData, &swapchainCreateInfo) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create swapchain data for surface %p", surface);
        goto fail_cleanup;
    }

    surface->surface = vkSurface;
    surface->swapchainData = swapchainData;

    return Lvn_Result_Success;

fail_cleanup:
    vkBackends->destroySurfaceKHR(vkBackends->instance, vkSurface, NULL);
    vkBackends->destroySwapchainKHR(vkBackends->device, swapchainData->swapchain, NULL);
    lvn_free(swapchainData->swapchainImages);
    lvn_free(swapchainData);
    return Lvn_Result_Failure;
}

void lvnImplVkDestroySurface(LvnSurface* surface)
{
    LVN_ASSERT(surface, "surface cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) surface->graphicsctx->implData;
    LvnVkSwapchainData* swapchainData = (LvnVkSwapchainData*) surface->swapchainData;

    // swapchain image views
    for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
        vkBackends->destroyImageView(vkBackends->device, swapchainData->swapchainImageViews[i], NULL);
    lvn_free(swapchainData->swapchainImageViews);

    // swapchain images
    lvn_free(swapchainData->swapchainImages);

    // swapchain
    vkBackends->destroySwapchainKHR(vkBackends->device, swapchainData->swapchain, NULL);

    // swapchain data struct
    lvn_free(surface->swapchainData);
    surface->swapchainData = NULL;

    // surface
    VkSurfaceKHR vkSurface = (VkSurfaceKHR) surface->surface;
    vkBackends->destroySurfaceKHR(vkBackends->instance, vkSurface, NULL);
    surface->surface = NULL;
}

LvnResult lvnImplVkCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkShaderModuleCreateInfo shaderCreateInfo = {0};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = createInfo->codeSize;
    shaderCreateInfo.pCode = (const uint32_t*) createInfo->pCode;

    VkShaderModule shaderModule;
    if (vkBackends->createShaderModule(vkBackends->device, &shaderCreateInfo, NULL, &shaderModule) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create shader module!");
        return Lvn_Result_Failure;
    }

    shader->shader = shaderModule;
    return Lvn_Result_Success;
}

void lvnImplVkDestroyShader(LvnShader* shader)
{
    LVN_ASSERT(shader, "shader cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) shader->graphicsctx->implData;
    VkShaderModule shaderModule = (VkShaderModule) shader->shader;
    vkBackends->destroyShaderModule(vkBackends->device, shaderModule, NULL);
    shader->shader = NULL;
}

LvnResult lvnImplVkCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo)
{
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

}

void lvnImplVkDestroyPipeline(LvnPipeline* pipeline)
{

}
