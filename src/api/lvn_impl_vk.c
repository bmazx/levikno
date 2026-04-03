#include "lvn_impl_vk.h"
#include "lvn_impl_vk_backends.h"

#include <stdlib.h>
#include <string.h>


#if defined(LVN_INCLUDE_WAYLAND) || defined(LVN_INCLUDE_X11)
    #if defined(LVN_INCLUDE_WAYLAND)
        #include <vulkan/vulkan_wayland.h>
    #endif
    #if defined(LVN_INCLUDE_X11)
        #include <X11/Xlib.h>
        #include <vulkan/vulkan_xlib.h>
    #endif
#endif

#include <vk_mem_alloc.h>


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

static const char* s_LvnVkDeviceExtensions[] =
{
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
};

static void                        lvn_getWindowPlatform(bool* useWayland, bool* useX11);
static PFN_vkVoidFunction          lvn_getVulkanCreateSurfaceProcAddr(const LvnVulkanBackends* vkBackends);
static LvnResult                   lvn_createPlatformSurface(const LvnVulkanBackends* vkBackends, VkSurfaceKHR* surface, const LvnPlatformData* platformData);
static LvnVkQueueFamilyIndices     lvn_findQueueFamilies(const LvnVulkanBackends* vkBackends, VkPhysicalDevice device, VkSurfaceKHR surface);
static bool                        lvn_checkDeviceExtensionSupport(const LvnVulkanBackends* vkBackends, VkPhysicalDevice device, const char** requiredExtensions, uint32_t requiredExtensionCount);
static VkPhysicalDevice            lvn_getBestPhysicalDevice(const LvnVulkanBackends* vkBackends, VkSurfaceKHR surface);
static LvnResult                   lvn_createSwapChainData(const LvnVulkanBackends* vkBackends, LvnVkSwapchainData* swapchainData, const LvnVkSwapChainCreateInfo* createInfo);
static VkShaderStageFlagBits       lvn_getVkShaderStageEnum(LvnShaderStage stage);
static VkFormat                    lvn_getVkVertexAttributeFormatEnum(LvnAttributeFormat format);
static VkPrimitiveTopology         lvn_getVkTopologyTypeEnum(LvnTopologyType topologyType);
static VkCullModeFlags             lvn_getVkCullModeFlagEnum(LvnCullFaceMode cullFaceMode);
static VkFrontFace                 lvn_getVkCullFrontFaceEnum(LvnCullFrontFace cullFrontFace);
static VkSampleCountFlagBits       lvn_getVkSampleCountFlagEnum(LvnSampleCountFlagBits samples);
static VkColorComponentFlags       lvn_getVkColorComponentsFlagEnum(LvnColorComponentFlags colorMask);
static VkBlendFactor               lvn_getVkBlendFactorEnum(LvnColorBlendFactor blendFactor);
static VkBlendOp                   lvn_getVkBlendOperationEnum(LvnColorBlendOperation blendOp);
static VkCompareOp                 lvn_getVkCompareOpEnum(LvnCompareOperation compare);
static VkStencilOp                 lvn_getVkStencilOpEnum(LvnStencilOperation stencilOp);
static VkAttachmentLoadOp          lvn_getVkAttackmentLoadOpEnum(LvnAttachmentLoadOp loadOp);
static VkAttachmentStoreOp         lvn_getVkAttackmentStoreOpEnum(LvnAttachmentStoreOp storeOp);
static VkCommandBufferLevel        lvn_getVkCommandBufferLevelEnum(LvnCommandBufferLevel level);
static VkFormat                    lvn_getVkFormatEnum(LvnFormat format);
static VkPresentModeKHR            lvn_getVkPresentModeEnum(LvnPresentMode presentMode);
static VkFilter                    lvn_getVkTextureFilterEnum(LvnTextureFilter filter);
static VkSamplerAddressMode        lvn_getVkTextureModeEnum(LvnTextureMode mode);
static LvnFormat                   lvn_getLvnFormatEnum(VkFormat format);
static LvnPresentMode              lvn_getLvnPresentModeEnum(VkPresentModeKHR presentMode);
static void                        lvn_transitionImageLayout(const LvnVulkanBackends* vkBackends, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount);
static LvnResult                   lvn_createBuffer(const LvnVulkanBackends* vkBackends, VkBuffer* buffer, VmaAllocation* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage);
static void                        lvn_copyBuffer(const LvnVulkanBackends* vkBackends, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset);
static LvnResult                   lvn_createImage(const LvnVulkanBackends* vkBackends, VkImage* image, VmaAllocation* imageMemory, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlagBits samples, VmaMemoryUsage memUsage);
static void                        lvn_copyBufferToImage(const LvnVulkanBackends* vkBackends, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

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

static void lvn_getWindowPlatform(bool* useWayland, bool* useX11)
{
    LVN_ASSERT(useWayland && useX11, "useWayland and useX11 cannot be null");

#if defined(LVN_INCLUDE_WAYLAND) || defined(LVN_INCLUDE_X11)
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session && (strcmp(session, "wayland") == 0 || strcmp(session, "x11") == 0))
    {
        if (strcmp(session, "wayland") == 0)
            *useWayland = true;
        else if (strcmp(session, "x11") == 0)
            *useX11 = true;
    }
    else
    {
        const char* waylandenv = getenv("WAYLAND_DISPLAY");
        const char* x11env = getenv("DISPLAY");
        if (waylandenv)
            *useWayland = true;
        else if (x11env)
            *useX11 = true;
    }
#else
    *useWayland = false;
    *useX11 = false;
#endif
}

static PFN_vkVoidFunction lvn_getVulkanCreateSurfaceProcAddr(const LvnVulkanBackends* vkBackends)
{
    LVN_ASSERT(vkBackends, "vkBackends cannot be null");

    bool useWayland = false, useX11 = false;
    lvn_getWindowPlatform(&useWayland, &useX11);
#if defined(LVN_INCLUDE_WAYLAND)
    if (useWayland)
        return vkBackends->getInstanceProcAddr(vkBackends->instance, "vkCreateWaylandSurfaceKHR");
#endif
#if defined(LVN_INCLUDE_X11)
    if (useX11)
        return vkBackends->getInstanceProcAddr(vkBackends->instance, "vkCreateXlibSurfaceKHR");
#endif

    return VK_NULL_HANDLE;
}

static LvnResult lvn_createPlatformSurface(const LvnVulkanBackends* vkBackends, VkSurfaceKHR* surface, const LvnPlatformData* platformData)
{
    VkResult result = VK_ERROR_UNKNOWN;
    bool useWayland = false, useX11 = false;
    lvn_getWindowPlatform(&useWayland, &useX11);

#if defined(LVN_INCLUDE_WAYLAND)
    if (useWayland)
    {
        VkWaylandSurfaceCreateInfoKHR sci = {
            .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
            .display = (struct wl_display*) platformData->ndh,
            .surface = (struct wl_surface*) platformData->nwh,
        };
        PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR_PFN =
            (PFN_vkCreateWaylandSurfaceKHR) vkBackends->createSurfaceProc;
        result = vkCreateWaylandSurfaceKHR_PFN(vkBackends->instance, &sci, NULL, surface);
    }
#endif
#if defined(LVN_INCLUDE_X11)
    if (useX11)
    {
        VkXlibSurfaceCreateInfoKHR sci = {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .dpy = (Display*) platformData->ndh,
            .window = *(Window*) platformData->nwh,
        };
        PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR =
            (PFN_vkCreateXlibSurfaceKHR) vkBackends->createSurfaceProc;
        result = vkCreateXlibSurfaceKHR(vkBackends->instance, &sci, NULL, surface);
    }
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

    uint32_t requiredExtensionCount = LVN_ARRAY_LEN(s_LvnVkDeviceExtensions);
    const char** requiredExtensions = lvn_calloc(requiredExtensionCount * sizeof(const char*));
    memcpy(requiredExtensions, s_LvnVkDeviceExtensions, sizeof(s_LvnVkDeviceExtensions));

    // get device extensions for surface present support
    if (surface)
    {
        requiredExtensions = lvn_realloc(requiredExtensions, ++requiredExtensionCount);
        requiredExtensions[requiredExtensionCount - 1] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
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
        if (!lvn_checkDeviceExtensionSupport(vkBackends, physicalDevice, requiredExtensions, requiredExtensionCount))
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
    lvn_free(requiredExtensions);

    return bestDevice;
}

static LvnResult lvn_createSwapChainData(const LvnVulkanBackends* vkBackends, LvnVkSwapchainData* swapchainData, const LvnVkSwapChainCreateInfo* createInfo)
{
    LVN_ASSERT(vkBackends && swapchainData && createInfo, "vkBackends, swapchain, and createInfo cannot be null");
    LVN_ASSERT(createInfo->surface && createInfo->physicalDevice && createInfo->queueFamilyIndices, "createInfo->surface, createInfo->physicalDevice, and createInfo->queueFamilyIndices cannot be null");

    VkPresentModeKHR* presentModes = NULL;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkImage* swapchainImages = NULL;
    VkImageView* swapchainImageViews = NULL;
    uint32_t swapchainImageCount = 0;

    // check for swapchain capabilitie support
    VkSurfaceCapabilitiesKHR capabilities;
    vkBackends->getPhysicalDeviceSurfaceCapabilitiesKHR(createInfo->physicalDevice, createInfo->surface, &capabilities);

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

    // find desired present mode
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    bool presentModeDefault = true;
    for (uint32_t i = 0; i < presentModeCount; i++)
    {
        if (presentModes[i] == createInfo->presentMode)
        {
            presentMode = presentModes[i];
            presentModeDefault = false;
            break;
        }
    }

    if (presentModeDefault)
    {
        LVN_LOG_WARN(vkBackends->graphicsctx->coreLogger,
                     "[vulkan] unable to find desired present mode (VkPresentModeKHR) %d, fallback to supported vulkan present mode %d",
                     createInfo->presentMode,
                     presentMode);
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
    uint32_t imageCount = (createInfo->minImageCount != 0) ? createInfo->minImageCount : 3;

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

    if (createInfo->minImageCount && imageCount < createInfo->minImageCount)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger,
                      "[vulkan] failed to create swapchain, image count (%u) is less than minImageCount (%u) specified ",
                      imageCount,
                      createInfo->minImageCount);
        goto fail_cleanup;
    }

    // create swapchain
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {0};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = createInfo->surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = createInfo->surfaceFormat;
    swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // NOTE: support other colorspace in future?
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform = capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = swapchainData->oldSwapchain; // use for recreating swapchain on window resize

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
        imageViewCreateInfo.format = createInfo->surfaceFormat;
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

    swapchainData->surface = createInfo->surface;
    swapchainData->swapchain = swapchain;
    swapchainData->swapchainFormat = createInfo->surfaceFormat;
    swapchainData->presentMode = presentMode;
    swapchainData->swapchainExtent = extent;
    swapchainData->swapchainImageCount = swapchainImageCount;
    swapchainData->swapchainImages = swapchainImages;
    swapchainData->swapchainImageViews = swapchainImageViews;

    lvn_free(presentModes);

    return Lvn_Result_Success;

fail_cleanup:
    for (uint32_t i = 0; i < swapchainImageCount; i++)
        vkBackends->destroyImageView(vkBackends->device, swapchainImageViews[i], NULL);
    vkBackends->destroySwapchainKHR(vkBackends->device, swapchain, NULL);
    lvn_free(swapchainImageViews);
    lvn_free(swapchainImages);
    lvn_free(presentModes);
    return Lvn_Result_Failure;
}

static VkShaderStageFlagBits lvn_getVkShaderStageEnum(LvnShaderStage stage)
{
    switch (stage)
    {
        case Lvn_ShaderStage_Vertex: { return VK_SHADER_STAGE_VERTEX_BIT; }
        case Lvn_ShaderStage_Fragment: { return VK_SHADER_STAGE_FRAGMENT_BIT; }
    }

    LVN_ASSERT(false, "invalid shader stage enum");
    return VK_SHADER_STAGE_VERTEX_BIT;
}

static VkFormat lvn_getVkVertexAttributeFormatEnum(LvnAttributeFormat format)
{
    switch (format)
    {
        case Lvn_AttributeFormat_Undefined:        { return VK_FORMAT_UNDEFINED; }
        case Lvn_AttributeFormat_Scalar_f32:       { return VK_FORMAT_R32_SFLOAT; }
        case Lvn_AttributeFormat_Scalar_f64:       { return VK_FORMAT_R64_SFLOAT; }
        case Lvn_AttributeFormat_Scalar_i32:       { return VK_FORMAT_R32_SINT; }
        case Lvn_AttributeFormat_Scalar_ui32:      { return VK_FORMAT_R32_UINT; }
        case Lvn_AttributeFormat_Scalar_i8:        { return VK_FORMAT_R8_SINT; }
        case Lvn_AttributeFormat_Scalar_ui8:       { return VK_FORMAT_R8_UINT; }
        case Lvn_AttributeFormat_Vec2_f32:         { return VK_FORMAT_R32G32_SFLOAT; }
        case Lvn_AttributeFormat_Vec3_f32:         { return VK_FORMAT_R32G32B32_SFLOAT; }
        case Lvn_AttributeFormat_Vec4_f32:         { return VK_FORMAT_R32G32B32A32_SFLOAT; }
        case Lvn_AttributeFormat_Vec2_f64:         { return VK_FORMAT_R64G64_SFLOAT; }
        case Lvn_AttributeFormat_Vec3_f64:         { return VK_FORMAT_R64G64B64_SFLOAT; }
        case Lvn_AttributeFormat_Vec4_f64:         { return VK_FORMAT_R64G64B64A64_SFLOAT; }
        case Lvn_AttributeFormat_Vec2_i32:         { return VK_FORMAT_R32G32_SINT; }
        case Lvn_AttributeFormat_Vec3_i32:         { return VK_FORMAT_R32G32B32_SINT; }
        case Lvn_AttributeFormat_Vec4_i32:         { return VK_FORMAT_R32G32B32A32_SINT; }
        case Lvn_AttributeFormat_Vec2_ui32:        { return VK_FORMAT_R32G32_UINT; }
        case Lvn_AttributeFormat_Vec3_ui32:        { return VK_FORMAT_R32G32B32_UINT; }
        case Lvn_AttributeFormat_Vec4_ui32:        { return VK_FORMAT_R32G32B32A32_UINT; }
        case Lvn_AttributeFormat_Vec2_i8:          { return VK_FORMAT_R8G8_SINT; }
        case Lvn_AttributeFormat_Vec3_i8:          { return VK_FORMAT_R8G8B8_SINT; }
        case Lvn_AttributeFormat_Vec4_i8:          { return VK_FORMAT_R8G8B8A8_SINT; }
        case Lvn_AttributeFormat_Vec2_ui8:         { return VK_FORMAT_R8G8_UINT; }
        case Lvn_AttributeFormat_Vec3_ui8:         { return VK_FORMAT_R8G8B8_UINT; }
        case Lvn_AttributeFormat_Vec4_ui8:         { return VK_FORMAT_R8G8B8A8_UINT; }
        case Lvn_AttributeFormat_Vec2_n8:          { return VK_FORMAT_R8G8_SNORM; }
        case Lvn_AttributeFormat_Vec3_n8:          { return VK_FORMAT_R8G8B8_SNORM; }
        case Lvn_AttributeFormat_Vec4_n8:          { return VK_FORMAT_R8G8B8A8_SNORM; }
        case Lvn_AttributeFormat_Vec2_un8:         { return VK_FORMAT_R8G8_UNORM; }
        case Lvn_AttributeFormat_Vec3_un8:         { return VK_FORMAT_R8G8B8_UNORM; }
        case Lvn_AttributeFormat_Vec4_un8:         { return VK_FORMAT_R8G8B8A8_UNORM; }
        case Lvn_AttributeFormat_2_10_10_10_ile:   { return VK_FORMAT_A2B10G10R10_SINT_PACK32; }
        case Lvn_AttributeFormat_2_10_10_10_uile:  { return VK_FORMAT_A2B10G10R10_UINT_PACK32; }
        case Lvn_AttributeFormat_2_10_10_10_nle:   { return VK_FORMAT_A2B10G10R10_SNORM_PACK32; }
        case Lvn_AttributeFormat_2_10_10_10_unle:  { return VK_FORMAT_A2B10G10R10_UNORM_PACK32; }
    }

    LVN_ASSERT(false, "invalid vertex attribute format enum");
    return VK_FORMAT_UNDEFINED;
}

static VkPrimitiveTopology lvn_getVkTopologyTypeEnum(LvnTopologyType topologyType)
{
    switch (topologyType)
    {
        case Lvn_TopologyType_Point: { return VK_PRIMITIVE_TOPOLOGY_POINT_LIST; }
        case Lvn_TopologyType_Line: { return VK_PRIMITIVE_TOPOLOGY_LINE_LIST; }
        case Lvn_TopologyType_LineStrip: { return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; }
        case Lvn_TopologyType_Triangle: { return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; }
        case Lvn_TopologyType_TriangleStrip: { return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; }
    }

    LVN_ASSERT(false, "invalid topology type enum");
    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
}

static VkCullModeFlags lvn_getVkCullModeFlagEnum(LvnCullFaceMode cullFaceMode)
{
    switch (cullFaceMode)
    {
        case Lvn_CullFaceMode_Disable: { return VK_CULL_MODE_NONE; }
        case Lvn_CullFaceMode_Front: { return VK_CULL_MODE_FRONT_BIT; }
        case Lvn_CullFaceMode_Back: { return VK_CULL_MODE_BACK_BIT; }
        case Lvn_CullFaceMode_Both: { return VK_CULL_MODE_FRONT_AND_BACK; }
    }

    LVN_ASSERT(false, "invalid cull face mode enum");
    return VK_CULL_MODE_NONE;
}

static VkFrontFace lvn_getVkCullFrontFaceEnum(LvnCullFrontFace cullFrontFace)
{
    switch (cullFrontFace)
    {
        case Lvn_CullFrontFace_Clockwise: { return VK_FRONT_FACE_CLOCKWISE; }
        case Lvn_CullFrontFace_CounterClockwise: { return VK_FRONT_FACE_COUNTER_CLOCKWISE; }
    }

    LVN_ASSERT(false, "invalid cull front face enum");
    return VK_FRONT_FACE_CLOCKWISE;
}

static VkSampleCountFlagBits lvn_getVkSampleCountFlagEnum(LvnSampleCountFlagBits samples)
{
    switch (samples)
    {
        case Lvn_SampleCountFlag_1_Bit: { return VK_SAMPLE_COUNT_1_BIT; }
        case Lvn_SampleCountFlag_2_Bit: { return VK_SAMPLE_COUNT_2_BIT; }
        case Lvn_SampleCountFlag_4_Bit: { return VK_SAMPLE_COUNT_4_BIT; }
        case Lvn_SampleCountFlag_8_Bit: { return VK_SAMPLE_COUNT_8_BIT; }
        case Lvn_SampleCountFlag_16_Bit: { return VK_SAMPLE_COUNT_16_BIT; }
        case Lvn_SampleCountFlag_32_Bit: { return VK_SAMPLE_COUNT_32_BIT; }
        case Lvn_SampleCountFlag_64_Bit: { return VK_SAMPLE_COUNT_64_BIT; }
    }

    LVN_ASSERT(false, "invalid sampler count enum");
    return VK_SAMPLE_COUNT_1_BIT;
}

static VkColorComponentFlags lvn_getVkColorComponentsFlagEnum(LvnColorComponentFlags colorMask)
{
    VkColorComponentFlags colorComponentsFlag = 0;

    if (colorMask & Lvn_ColorComponentFlag_R) colorComponentsFlag |= VK_COLOR_COMPONENT_R_BIT;
    if (colorMask & Lvn_ColorComponentFlag_G) colorComponentsFlag |= VK_COLOR_COMPONENT_G_BIT;
    if (colorMask & Lvn_ColorComponentFlag_B) colorComponentsFlag |= VK_COLOR_COMPONENT_B_BIT;
    if (colorMask & Lvn_ColorComponentFlag_A) colorComponentsFlag |= VK_COLOR_COMPONENT_A_BIT;

    return colorComponentsFlag;
}

static VkBlendFactor lvn_getVkBlendFactorEnum(LvnColorBlendFactor blendFactor)
{
    switch (blendFactor)
    {
        case Lvn_ColorBlendFactor_Zero:                  { return VK_BLEND_FACTOR_ZERO; }
        case Lvn_ColorBlendFactor_One:                   { return VK_BLEND_FACTOR_ONE; }
        case Lvn_ColorBlendFactor_SrcColor:              { return VK_BLEND_FACTOR_SRC_COLOR; }
        case Lvn_ColorBlendFactor_OneMinusSrcColor:      { return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; }
        case Lvn_ColorBlendFactor_DstColor:              { return VK_BLEND_FACTOR_DST_COLOR; }
        case Lvn_ColorBlendFactor_OneMinusDstColor:      { return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; }
        case Lvn_ColorBlendFactor_SrcAlpha:              { return VK_BLEND_FACTOR_SRC_ALPHA; }
        case Lvn_ColorBlendFactor_OneMinusSrcAlpha:      { return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; }
        case Lvn_ColorBlendFactor_DstAlpha:              { return VK_BLEND_FACTOR_DST_ALPHA; }
        case Lvn_ColorBlendFactor_OneMinusDstAlpha:      { return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; }
        case Lvn_ColorBlendFactor_ConstantColor:         { return VK_BLEND_FACTOR_CONSTANT_COLOR; }
        case Lvn_ColorBlendFactor_OneMinusConstantColor: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR; }
        case Lvn_ColorBlendFactor_ConstantAlpha:         { return VK_BLEND_FACTOR_CONSTANT_ALPHA; }
        case Lvn_ColorBlendFactor_OneMinusConstantAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA; }
        case Lvn_ColorBlendFactor_SrcAlphaSaturate:      { return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; }
        case Lvn_ColorBlendFactor_Src1Color:             { return VK_BLEND_FACTOR_SRC1_COLOR; }
        case Lvn_ColorBlendFactor_OneMinusSrc1Color:     { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR; }
        case Lvn_ColorBlendFactor_Src1_Alpha:            { return VK_BLEND_FACTOR_SRC1_ALPHA; }
        case Lvn_ColorBlendFactor_OneMinusSrc1Alpha:     { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA; }
    }

    LVN_ASSERT(false, "invalid blend factor enum");
    return VK_BLEND_FACTOR_ZERO;
}

static VkBlendOp lvn_getVkBlendOperationEnum(LvnColorBlendOperation blendOp)
{
    switch (blendOp)
    {
        case Lvn_ColorBlendOp_Add: { return VK_BLEND_OP_ADD; }
        case Lvn_ColorBlendOp_Subtract: { return VK_BLEND_OP_SUBTRACT; }
        case Lvn_ColorBlendOp_ReverseSubtract: { return VK_BLEND_OP_REVERSE_SUBTRACT; }
        case Lvn_ColorBlendOp_Min: { return VK_BLEND_OP_MIN; }
        case Lvn_ColorBlendOp_Max: { return VK_BLEND_OP_MAX; }
    }

    LVN_ASSERT(false, "invalid blend operation enum");
    return VK_BLEND_OP_ADD;
}

static VkCompareOp lvn_getVkCompareOpEnum(LvnCompareOperation compare)
{
    switch (compare)
    {
        case Lvn_CompareOp_Never: { return VK_COMPARE_OP_NEVER; }
        case Lvn_CompareOp_Less: { return VK_COMPARE_OP_LESS; }
        case Lvn_CompareOp_Equal: { return VK_COMPARE_OP_EQUAL; }
        case Lvn_CompareOp_LessOrEqual: { return VK_COMPARE_OP_LESS_OR_EQUAL; }
        case Lvn_CompareOp_Greater: { return VK_COMPARE_OP_GREATER; }
        case Lvn_CompareOp_NotEqual: { return VK_COMPARE_OP_NOT_EQUAL; }
        case Lvn_CompareOp_GreaterOrEqual: { return VK_COMPARE_OP_GREATER_OR_EQUAL; }
        case Lvn_CompareOp_Always: { return VK_COMPARE_OP_ALWAYS; }
    }

    LVN_ASSERT(false, "invalid compare enum");
    return VK_COMPARE_OP_NEVER;
}

static VkStencilOp lvn_getVkStencilOpEnum(LvnStencilOperation stencilOp)
{
    switch (stencilOp)
    {
        case Lvn_StencilOp_Keep: { return VK_STENCIL_OP_KEEP; }
        case Lvn_StencilOp_Zero: { return VK_STENCIL_OP_ZERO; }
        case Lvn_StencilOp_Replace: { return VK_STENCIL_OP_REPLACE; }
        case Lvn_StencilOp_IncrementAndClamp: { return VK_STENCIL_OP_INCREMENT_AND_CLAMP; }
        case Lvn_StencilOp_DecrementAndClamp: { return VK_STENCIL_OP_DECREMENT_AND_CLAMP; }
        case Lvn_StencilOp_Invert: { return VK_STENCIL_OP_INVERT; }
        case Lvn_StencilOp_IncrementAndWrap: { return VK_STENCIL_OP_INCREMENT_AND_WRAP; }
        case Lvn_StencilOp_DecrementAndWrap: { return VK_STENCIL_OP_DECREMENT_AND_WRAP; }
    }

    LVN_ASSERT(false, "invalid stencil operation enum");
    return VK_STENCIL_OP_KEEP;
}

static VkAttachmentLoadOp lvn_getVkAttackmentLoadOpEnum(LvnAttachmentLoadOp loadOp)
{
    switch (loadOp)
    {
        case Lvn_AttachmentLoadOp_Load: { return VK_ATTACHMENT_LOAD_OP_LOAD; }
        case Lvn_AttachmentLoadOp_Clear: { return VK_ATTACHMENT_LOAD_OP_CLEAR; }
        case Lvn_AttachmentLoadOp_DontCare: { return VK_ATTACHMENT_LOAD_OP_DONT_CARE; }
    }

    LVN_ASSERT(false, "invalid attachment load operator enum");
    return VK_ATTACHMENT_LOAD_OP_LOAD;
}

static VkAttachmentStoreOp lvn_getVkAttackmentStoreOpEnum(LvnAttachmentStoreOp storeOp)
{
    switch (storeOp)
    {
        case Lvn_AttachmentStoreOp_Store: { return VK_ATTACHMENT_STORE_OP_STORE; }
        case Lvn_AttachmentStoreOp_DontCare: { return VK_ATTACHMENT_STORE_OP_DONT_CARE; }
    }

    LVN_ASSERT(false, "invalid attachment store operator enum");
    return VK_ATTACHMENT_STORE_OP_STORE;
}

static VkImageLayout lvn_getVkImageLayoutEnumUsage(LvnAttachmentUsage usage)
{
    switch (usage)
    {
        case Lvn_AttachmentUsage_ColorAttachment: { return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; }
        case Lvn_AttachmentUsage_ShaderReadOnly: { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }
        case Lvn_AttachmentUsage_PresentSrc: { return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; }
    }

    LVN_ASSERT(false, "invalid attachment usage (VkImageLayout) enum");
    return VK_IMAGE_LAYOUT_UNDEFINED;
}

static VkCommandBufferLevel lvn_getVkCommandBufferLevelEnum(LvnCommandBufferLevel level)
{
    switch (level)
    {
        case Lvn_CommandBufferLevel_Primary: { return VK_COMMAND_BUFFER_LEVEL_PRIMARY; }
        case Lvn_CommandBufferLevel_Secondary: { return VK_COMMAND_BUFFER_LEVEL_SECONDARY; }
    }

    LVN_ASSERT(false, "invalid command buffer level enum");
    return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
}

static VkFormat lvn_getVkFormatEnum(LvnFormat format)
{
    switch (format)
    {
        case Lvn_Format_None: { return VK_FORMAT_UNDEFINED; }
        case Lvn_Format_R8G8B8_UNORM: { return VK_FORMAT_R8G8B8_UNORM; }
        case Lvn_Format_R8G8B8_SRGB: { return VK_FORMAT_R8G8B8_SRGB; }
        case Lvn_Format_R8G8B8A8_UNORM: { return VK_FORMAT_R8G8B8A8_UNORM; }
        case Lvn_Format_R8G8B8A8_SRGB: { return VK_FORMAT_R8G8B8A8_SRGB; }
        case Lvn_Format_B8G8R8_SRGB: { return VK_FORMAT_B8G8R8_SRGB; }
        case Lvn_Format_B8G8R8A8_SRGB: { return VK_FORMAT_B8G8R8A8_SRGB; }
    }

    LVN_ASSERT(false, "invalid format enum");
    return VK_FORMAT_UNDEFINED;
}

static VkPresentModeKHR lvn_getVkPresentModeEnum(LvnPresentMode presentMode)
{
    switch (presentMode)
    {
        case Lvn_PresentMode_FIFO: { return VK_PRESENT_MODE_FIFO_KHR; }
        case Lvn_PresentMode_Mailbox: { return VK_PRESENT_MODE_MAILBOX_KHR; }
        case Lvn_PresentMode_Immediate: { return VK_PRESENT_MODE_IMMEDIATE_KHR; }
    }

    LVN_ASSERT(false, "invalid present mode enum");
    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkFilter lvn_getVkTextureFilterEnum(LvnTextureFilter filter)
{
    switch (filter)
    {
        case Lvn_TextureFilter_Nearest: { return VK_FILTER_NEAREST; }
        case Lvn_TextureFilter_Linear: { return VK_FILTER_LINEAR; }
    }

    LVN_ASSERT(false, "invalid texture filter enum");
    return VK_FILTER_NEAREST;
}

static VkSamplerAddressMode lvn_getVkTextureModeEnum(LvnTextureMode mode)
{
    switch (mode)
    {
        case Lvn_TextureMode_Repeat: { return VK_SAMPLER_ADDRESS_MODE_REPEAT; }
        case Lvn_TextureMode_MirrorRepeat: { return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; }
        case Lvn_TextureMode_ClampToEdge: { return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; }
        case Lvn_TextureMode_ClampToBorder: { return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; }
    }

    LVN_ASSERT(false, "invalid wrap mode enum");
    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
}

static LvnFormat lvn_getLvnFormatEnum(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_UNDEFINED: { return Lvn_Format_None; }
        case VK_FORMAT_R8G8B8_UNORM: { return Lvn_Format_R8G8B8_UNORM; }
        case VK_FORMAT_R8G8B8_SRGB: { return Lvn_Format_R8G8B8_SRGB; }
        case VK_FORMAT_R8G8B8A8_UNORM: { return Lvn_Format_R8G8B8A8_UNORM; }
        case VK_FORMAT_R8G8B8A8_SRGB: { return Lvn_Format_R8G8B8A8_SRGB; }
        case VK_FORMAT_B8G8R8_SRGB: { return Lvn_Format_B8G8R8_SRGB; }
        case VK_FORMAT_B8G8R8A8_SRGB: { return Lvn_Format_B8G8R8A8_SRGB; }
        default: { break; }
    }

    return Lvn_Format_None;
}

static LvnPresentMode lvn_getLvnPresentModeEnum(VkPresentModeKHR presentMode)
{
    switch (presentMode)
    {
        case VK_PRESENT_MODE_FIFO_KHR: { return Lvn_PresentMode_FIFO; }
        case VK_PRESENT_MODE_MAILBOX_KHR: { return Lvn_PresentMode_Mailbox; }
        case VK_PRESENT_MODE_IMMEDIATE_KHR: { return Lvn_PresentMode_Immediate; }
        default: { break; }
    }

    return Lvn_PresentMode_FIFO;
}

static void lvn_transitionImageLayout(
    const LvnVulkanBackends* vkBackends,
    VkImage image,
    VkFormat format,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    uint32_t layerCount)
{
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = vkBackends->commandPool,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    vkBackends->allocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBackends->beginCommandBuffer(commandBuffer, &beginInfo);

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = layerCount,
    };

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        // stencil
        if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger,
                      "[vulkan] unsupported layout transition during image layout transition");
        vkBackends->freeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
        return;
    }

    vkBackends->cmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
    vkBackends->endCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vkBackends->queueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkBackends->queueWaitIdle(vkBackends->graphicsQueue);

    vkBackends->freeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
}

static LvnResult lvn_createBuffer(
    const LvnVulkanBackends* vkBackends,
    VkBuffer* buffer,
    VmaAllocation* bufferMemory,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VmaMemoryUsage memUsage)
{
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VmaAllocationCreateInfo allocInfo = {
        .usage = memUsage,
    };

    if (vmaCreateBuffer(vkBackends->vmaAllocator, &bufferInfo, &allocInfo, buffer, bufferMemory, NULL) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger,
                      "[vulkan] failed to create buffer <VkBuffer> (%p), buffer memory: (%p), buffer size: %zu bytes",
                      *buffer, *bufferMemory, size);
        return Lvn_Result_Failure;
    }

    return Lvn_Result_Success;
}

static void lvn_copyBuffer(
    const LvnVulkanBackends* vkBackends,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset)
{
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = vkBackends->commandPool,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    vkBackends->allocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBackends->beginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {
        .size = size,
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
    };

    vkBackends->cmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkBackends->endCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vkBackends->queueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkBackends->queueWaitIdle(vkBackends->graphicsQueue);

    vkBackends->freeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
}

static LvnResult lvn_createImage(
    const LvnVulkanBackends* vkBackends,
    VkImage* image,
    VmaAllocation* imageMemory,
    uint32_t width,
    uint32_t height,
    VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkSampleCountFlagBits samples,
    VmaMemoryUsage memUsage)
{
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent.width = width,
        .extent.height = height,
        .extent.depth = 1,
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = format,
        .tiling = tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = usage,
        .samples = samples,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VmaAllocationCreateInfo allocInfo = {
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY,
    };

    if (vmaCreateImage(vkBackends->vmaAllocator, &imageInfo, &allocInfo, image, imageMemory, NULL) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger,
                      "[vulkan] failed to create image <VkImage>, image size: (w:%u, h:%u)",
                      width, height);
        return Lvn_Result_Failure;
    }

    return Lvn_Result_Success;
}

static void lvn_copyBufferToImage(
    const LvnVulkanBackends* vkBackends,
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height,
    uint32_t layerCount)
{
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = vkBackends->commandPool,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    vkBackends->allocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBackends->beginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferImageCopy region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .imageSubresource.mipLevel = 0,
        .imageSubresource.baseArrayLayer = 0,
        .imageSubresource.layerCount = layerCount,
        .imageOffset = { 0, 0, 0 },
        .imageExtent = { width, height, 1 },
    };

    vkBackends->cmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    vkBackends->endCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vkBackends->queueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkBackends->queueWaitIdle(vkBackends->graphicsQueue);

    vkBackends->freeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
}

LvnResult lvnImplVkInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && createInfo, "graphicsctx and createInfo cannot be nullptr");

    const char** extensionNames = NULL;
    VkExtensionProperties* extensionProps = NULL;
    VkLayerProperties* availableLayers = NULL;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    LvnVulkanBackends* vkBackends = (LvnVulkanBackends*) lvn_calloc(sizeof(LvnVulkanBackends));
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

    // vulkan get instace proc address
    vkBackends->getInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
        lvn_platformGetModuleSymbol(vkBackends->handle, "vkGetInstanceProcAddr");

    if (!vkBackends->getInstanceProcAddr)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] unable to retrieve vkGetInstanceProcAddr symbol");
        goto fail_cleanup;
    }

    // vulkan global level function symbols
    vkBackends->enumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)
        vkBackends->getInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
    vkBackends->enumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)
        vkBackends->getInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
    vkBackends->enumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)
        vkBackends->getInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");
    vkBackends->createInstance = (PFN_vkCreateInstance)
        vkBackends->getInstanceProcAddr(NULL, "vkCreateInstance");


    if (!vkBackends->enumerateInstanceVersion ||
        !vkBackends->enumerateInstanceExtensionProperties ||
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

        extensionProps = (VkExtensionProperties*) lvn_calloc(extensionPropsCount * sizeof(VkExtensionProperties));
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
            extensionNames = (const char**) lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_surface";
        }
        if (vkBackends->ext.KHR_win32_surface)
        {
            extensionNames = (const char**) lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_win32_surface";
        }
        if (vkBackends->ext.MVK_macos_surface)
        {
            extensionNames = (const char**) lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_MVK_macos_surface";
        }
        if (vkBackends->ext.EXT_metal_surface)
        {
            extensionNames = (const char**) lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_EXT_metal_surface";
        }
        if (vkBackends->ext.KHR_xlib_surface)
        {
            extensionNames = (const char**) lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_xlib_surface";
        }
        if (vkBackends->ext.KHR_xcb_surface)
        {
            extensionNames = (const char**) lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_xcb_surface";
        }
        if (vkBackends->ext.KHR_wayland_surface)
        {
            extensionNames = (const char**) lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_KHR_wayland_surface";
        }
        if (vkBackends->ext.EXT_headless_surface)
        {
            extensionNames = (const char**) lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
            extensionNames[extensionCount - 1] = "VK_EXT_headless_surface";
        }
    }

    // check validation layer support
    bool layerSupport = true;
    uint32_t availableLayerCount = 0;
    if (vkBackends->enableValidationLayers)
    {
        vkBackends->enumerateInstanceLayerProperties(&availableLayerCount, NULL);
        availableLayers = (VkLayerProperties*) lvn_calloc(availableLayerCount * sizeof(VkLayerProperties));
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
            extensionNames = (const char**) lvn_realloc(extensionNames, ++extensionCount * sizeof(const char*));
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

    // get vulkan version
    uint32_t vulkanVersion;
    vkBackends->enumerateInstanceVersion(&vulkanVersion);
    vkBackends->versionMajor = VK_VERSION_MAJOR(vulkanVersion);
    vkBackends->versionMinor = VK_VERSION_MINOR(vulkanVersion);

    // create vulkan instance
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = graphicsctx->ctx->appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = NULL;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_MAKE_VERSION(vkBackends->versionMajor, vkBackends->versionMinor, 0);

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
    vkBackends->getPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceMemoryProperties");
    vkBackends->getPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceFormatProperties");
    vkBackends->getPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceFeatures");
    vkBackends->getDeviceProcAddr = (PFN_vkGetDeviceProcAddr)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetDeviceProcAddr");
    vkBackends->createDevice = (PFN_vkCreateDevice)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkCreateDevice");

    if (!vkBackends->destroyInstance ||
        !vkBackends->enumeratePhysicalDevices ||
        !vkBackends->getPhysicalDeviceQueueFamilyProperties ||
        !vkBackends->enumerateDeviceExtensionProperties ||
        !vkBackends->getPhysicalDeviceProperties ||
        !vkBackends->getPhysicalDeviceMemoryProperties ||
        !vkBackends->getPhysicalDeviceFeatures ||
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
        vkBackends->createSurfaceProc = lvn_getVulkanCreateSurfaceProcAddr(vkBackends);

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
    vkBackends->queueFamilyIndices = lvn_findQueueFamilies(vkBackends, vkBackends->physicalDevice, surface);
    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = vkBackends->queueFamilyIndices.graphicsIndex;
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
    vkBackends->createSampler = (PFN_vkCreateSampler)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateSampler");
    vkBackends->destroySampler = (PFN_vkDestroySampler)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroySampler");
    vkBackends->createShaderModule = (PFN_vkCreateShaderModule)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateShaderModule");
    vkBackends->destroyShaderModule = (PFN_vkDestroyShaderModule)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyShaderModule");
    vkBackends->createRenderPass = (PFN_vkCreateRenderPass)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateRenderPass");
    vkBackends->destroyRenderPass = (PFN_vkDestroyRenderPass)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyRenderPass");
    vkBackends->createPipelineLayout = (PFN_vkCreatePipelineLayout)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreatePipelineLayout");
    vkBackends->destroyPipelineLayout = (PFN_vkDestroyPipelineLayout)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyPipelineLayout");
    vkBackends->createGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateGraphicsPipelines");
    vkBackends->destroyPipeline = (PFN_vkDestroyPipeline)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyPipeline");
    vkBackends->createFramebuffer = (PFN_vkCreateFramebuffer)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateFramebuffer");
    vkBackends->destroyFramebuffer = (PFN_vkDestroyFramebuffer)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyFramebuffer");
    vkBackends->createBuffer = (PFN_vkCreateBuffer)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateBuffer");
    vkBackends->destroyBuffer = (PFN_vkDestroyBuffer)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyBuffer");
    vkBackends->createFence = (PFN_vkCreateFence)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateFence");
    vkBackends->destroyFence = (PFN_vkDestroyFence)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyFence");
    vkBackends->createSemaphore = (PFN_vkCreateSemaphore)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateSemaphore");
    vkBackends->destroySemaphore = (PFN_vkDestroySemaphore)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroySemaphore");
    vkBackends->createCommandPool = (PFN_vkCreateCommandPool)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCreateCommandPool");
    vkBackends->destroyCommandPool = (PFN_vkDestroyCommandPool)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDestroyCommandPool");
    vkBackends->allocateCommandBuffers = (PFN_vkAllocateCommandBuffers)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkAllocateCommandBuffers");
    vkBackends->freeCommandBuffers = (PFN_vkFreeCommandBuffers)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkFreeCommandBuffers");
    vkBackends->beginCommandBuffer = (PFN_vkBeginCommandBuffer)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkBeginCommandBuffer");
    vkBackends->endCommandBuffer = (PFN_vkEndCommandBuffer)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkEndCommandBuffer");
    vkBackends->cmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdBeginRenderPass");
    vkBackends->cmdEndRenderPass = (PFN_vkCmdEndRenderPass)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdEndRenderPass");
    vkBackends->cmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdPipelineBarrier");
    vkBackends->cmdBindPipeline = (PFN_vkCmdBindPipeline)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdBindPipeline");
    vkBackends->cmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdBindVertexBuffers");
    vkBackends->cmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdBindIndexBuffer");
    vkBackends->cmdSetViewport = (PFN_vkCmdSetViewport)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdSetViewport");
    vkBackends->cmdSetScissor = (PFN_vkCmdSetScissor)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdSetScissor");
    vkBackends->cmdDraw = (PFN_vkCmdDraw)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdDraw");
    vkBackends->cmdDrawIndexed = (PFN_vkCmdDrawIndexed)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdDrawIndexed");
    vkBackends->cmdCopyBuffer = (PFN_vkCmdCopyBuffer)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdCopyBuffer");
    vkBackends->cmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkCmdCopyBufferToImage");
    vkBackends->queueSubmit = (PFN_vkQueueSubmit)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkQueueSubmit");
    vkBackends->waitForFences = (PFN_vkWaitForFences)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkWaitForFences");
    vkBackends->resetFences = (PFN_vkResetFences)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkResetFences");
    vkBackends->deviceWaitIdle = (PFN_vkDeviceWaitIdle)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkDeviceWaitIdle");
    vkBackends->queueWaitIdle = (PFN_vkQueueWaitIdle)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkQueueWaitIdle");
    vkBackends->allocateMemory = (PFN_vkAllocateMemory)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkAllocateMemory");
    vkBackends->freeMemory = (PFN_vkFreeMemory)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkFreeMemory");
    vkBackends->mapMemory = (PFN_vkMapMemory)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkMapMemory");
    vkBackends->unmapMemory = (PFN_vkUnmapMemory)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkUnmapMemory");
    vkBackends->flushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkFlushMappedMemoryRanges");
    vkBackends->invalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkInvalidateMappedMemoryRanges");
    vkBackends->bindBufferMemory = (PFN_vkBindBufferMemory)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkBindBufferMemory");
    vkBackends->bindImageMemory = (PFN_vkBindImageMemory)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkBindImageMemory");
    vkBackends->getBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkGetBufferMemoryRequirements");
    vkBackends->getImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)
        vkBackends->getDeviceProcAddr(vkBackends->device, "vkGetImageMemoryRequirements");

    if (!vkBackends->destroyDevice ||
        !vkBackends->getDeviceQueue ||
        !vkBackends->createImage ||
        !vkBackends->destroyImage ||
        !vkBackends->createImageView ||
        !vkBackends->destroyImageView ||
        !vkBackends->createSampler ||
        !vkBackends->destroySampler ||
        !vkBackends->createShaderModule ||
        !vkBackends->destroyShaderModule ||
        !vkBackends->createRenderPass ||
        !vkBackends->destroyRenderPass ||
        !vkBackends->createPipelineLayout ||
        !vkBackends->destroyPipelineLayout ||
        !vkBackends->createGraphicsPipelines ||
        !vkBackends->destroyPipeline ||
        !vkBackends->createFramebuffer ||
        !vkBackends->destroyFramebuffer ||
        !vkBackends->createBuffer ||
        !vkBackends->destroyBuffer ||
        !vkBackends->createFence ||
        !vkBackends->destroyFence ||
        !vkBackends->createSemaphore ||
        !vkBackends->destroySemaphore ||
        !vkBackends->createCommandPool ||
        !vkBackends->destroyCommandPool ||
        !vkBackends->allocateCommandBuffers ||
        !vkBackends->beginCommandBuffer ||
        !vkBackends->endCommandBuffer ||
        !vkBackends->cmdBeginRenderPass ||
        !vkBackends->cmdEndRenderPass ||
        !vkBackends->cmdPipelineBarrier ||
        !vkBackends->cmdBindPipeline ||
        !vkBackends->cmdBindVertexBuffers ||
        !vkBackends->cmdBindIndexBuffer ||
        !vkBackends->cmdSetViewport ||
        !vkBackends->cmdSetScissor ||
        !vkBackends->cmdDraw ||
        !vkBackends->cmdDrawIndexed ||
        !vkBackends->cmdCopyBuffer ||
        !vkBackends->cmdCopyBufferToImage ||
        !vkBackends->queueSubmit ||
        !vkBackends->waitForFences ||
        !vkBackends->resetFences ||
        !vkBackends->deviceWaitIdle ||
        !vkBackends->allocateMemory ||
        !vkBackends->freeMemory ||
        !vkBackends->mapMemory ||
        !vkBackends->unmapMemory ||
        !vkBackends->flushMappedMemoryRanges ||
        !vkBackends->invalidateMappedMemoryRanges ||
        !vkBackends->bindBufferMemory ||
        !vkBackends->bindImageMemory ||
        !vkBackends->getBufferMemoryRequirements ||
        !vkBackends->getImageMemoryRequirements)
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
        vkBackends->acquireNextImageKHR = (PFN_vkAcquireNextImageKHR)
            vkBackends->getDeviceProcAddr(vkBackends->device, "vkAcquireNextImageKHR");
        vkBackends->queuePresentKHR = (PFN_vkQueuePresentKHR)
            vkBackends->getDeviceProcAddr(vkBackends->device, "vkQueuePresentKHR");

        if (!vkBackends->createSwapchainKHR ||
            !vkBackends->destroySwapchainKHR ||
            !vkBackends->getSwapchainImagesKHR ||
            !vkBackends->acquireNextImageKHR ||
            !vkBackends->queuePresentKHR)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to load vulkan device level surface function symbol");
            goto fail_cleanup;
        }

    }

    // get graphics and present queues from device
    vkBackends->getDeviceQueue(vkBackends->device, vkBackends->queueFamilyIndices.graphicsIndex, 0, &vkBackends->graphicsQueue);

    if (graphicsctx->presentModeFlags & Lvn_PresentationModeFlag_Surface)
        vkBackends->getDeviceQueue(vkBackends->device, vkBackends->queueFamilyIndices.presentIndex, 0, &vkBackends->presentQueue);


    // create command pool
    VkCommandPoolCreateInfo poolCreateInfo = {0};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolCreateInfo.queueFamilyIndex = vkBackends->queueFamilyIndices.graphicsIndex;
    if (vkBackends->createCommandPool(vkBackends->device, &poolCreateInfo, NULL, &vkBackends->commandPool) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create command pool");
        goto fail_cleanup;
    }

    // create vma allocator
    VmaVulkanFunctions vkFuncs = {
        .vkGetPhysicalDeviceProperties       = vkBackends->getPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties = vkBackends->getPhysicalDeviceMemoryProperties,
        .vkAllocateMemory                    = vkBackends->allocateMemory,
        .vkFreeMemory                        = vkBackends->freeMemory,
        .vkMapMemory                         = vkBackends->mapMemory,
        .vkUnmapMemory                       = vkBackends->unmapMemory,
        .vkFlushMappedMemoryRanges           = vkBackends->flushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges      = vkBackends->invalidateMappedMemoryRanges,
        .vkBindBufferMemory                  = vkBackends->bindBufferMemory,
        .vkBindImageMemory                   = vkBackends->bindImageMemory,
        .vkGetBufferMemoryRequirements       = vkBackends->getBufferMemoryRequirements,
        .vkGetImageMemoryRequirements        = vkBackends->getImageMemoryRequirements,
        .vkCreateBuffer                      = vkBackends->createBuffer,
        .vkDestroyBuffer                     = vkBackends->destroyBuffer,
        .vkCreateImage                       = vkBackends->createImage,
        .vkDestroyImage                      = vkBackends->destroyImage,
        .vkCmdCopyBuffer                     = vkBackends->cmdCopyBuffer,
    };

    VmaAllocatorCreateInfo allocatorInfo = {
        .device = vkBackends->device,
        .physicalDevice = vkBackends->physicalDevice,
        .instance = vkBackends->instance,
        .pVulkanFunctions = &vkFuncs,
    };

    if (vmaCreateAllocator(&allocatorInfo, &vkBackends->vmaAllocator) != VK_SUCCESS )
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vma] failed to create vma memory allocator for vulkan");
        goto fail_cleanup;
    }

    // set vulkan implementation function pointers
    graphicsctx->implCreateSurface = lvnImplVkCreateSurface;
    graphicsctx->implDestroySurface = lvnImplVkDestroySurface;
    graphicsctx->implCreateSwapchain = lvnImplVkCreateSwapchain;
    graphicsctx->implDestroySwapchain = lvnImplVkDestroySwapchain;
    graphicsctx->implCreateRenderPass = lvnImplVkCreateRenderPass;
    graphicsctx->implDestroyRenderPass = lvnImplVkDestroyRenderPass;
    graphicsctx->implCreateFramebuffer = lvnImplVkCreateFramebuffer;
    graphicsctx->implDestroyFramebuffer = lvnImplVkDestroyFramebuffer;
    graphicsctx->implCreateShader = lvnImplVkCreateShader;
    graphicsctx->implDestroyShader = lvnImplVkDestroyShader;
    graphicsctx->implCreatePipeline = lvnImplVkCreatePipeline;
    graphicsctx->implDestroyPipeline = lvnImplVkDestroyPipeline;
    graphicsctx->implCreateFence = lvnImplVkCreateFence;
    graphicsctx->implDestroyFence = lvnImplVkDestroyFence;
    graphicsctx->implCreateSemaphore = lvnImplVkCreateSemaphore;
    graphicsctx->implDestroySemaphore = lvnImplVkDestroySemaphore;
    graphicsctx->implCreateBuffer = lvnImplVksCreateBuffer;
    graphicsctx->implDestroyBuffer = lvnImplVksDestroyBuffer;
    graphicsctx->implAllocateCommandBuffers = lvnImplVkAllocateCommandBuffers;
    graphicsctx->implSurfaceGetSupportedFormats = lvnImplVkSurfaceGetSupportedFormats;
    graphicsctx->implSurfaceGetSupportedPresentModes = lvnImplVkSurfaceGetSupportedPresentModes;
    graphicsctx->implSwapchainResize = lvnImplVkSwapchainResize;
    graphicsctx->implSwapchainAcquireNextImage = lvnImplVkSwapchainAcquireNextImage;
    graphicsctx->implFenceWait = lvnImplVkFenceWait;
    graphicsctx->implFenceReset = lvnImplVkFenceReset;
    graphicsctx->implBufferUpdateData = lvnImplVkBufferUpdateData;
    graphicsctx->implBufferResize = lvnImplVkBufferResize;
    graphicsctx->implBeginCommandBuffer = lvnImplVkBeginCommandBuffer;
    graphicsctx->implEndCommandBuffer = lvnImplVkEndCommandBuffer;
    graphicsctx->implCmdBeginRenderPass = lvnImplVkCmdBeginRenderPass;
    graphicsctx->implCmdEndRenderPass = lvnImplVkCmdEndRenderPass;
    graphicsctx->implCmdBindPipeline = lvnImplVkCmdBindPipeline;
    graphicsctx->implCmdBindVertexBuffer = lvnImplVkCmdBindVertexBuffer;
    graphicsctx->implCmdBindIndexBuffer = lvnImplVkCmdBindIndexBuffer;
    graphicsctx->implCmdSetViewport = lvnImplVkCmdSetViewport;
    graphicsctx->implCmdSetScissor = lvnImplVkCmdSetScissor;
    graphicsctx->implCmdDraw = lvnImplVkCmdDraw;
    graphicsctx->implCmdDrawIndexed = lvnImplVkCmdDrawIndexed;
    graphicsctx->implRenderSubmit = lvnImplVkRenderSubmit;
    graphicsctx->implRenderPresent = lvnImplVkRenderPresent;

    vkBackends->destroySurfaceKHR(vkBackends->instance, surface, NULL);
    lvn_free(extensionProps);
    lvn_free(extensionNames);
    lvn_free(availableLayers);

    return Lvn_Result_Success;

fail_cleanup:
    vmaDestroyAllocator(vkBackends->vmaAllocator);
    vkBackends->destroyCommandPool(vkBackends->device, vkBackends->commandPool, NULL);
    vkBackends->destroySurfaceKHR(vkBackends->instance, surface, NULL);
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

    vkBackends->deviceWaitIdle(vkBackends->device);

    if (vkBackends->vmaAllocator)
        vmaDestroyAllocator(vkBackends->vmaAllocator);
    if (vkBackends->commandPool)
        vkBackends->destroyCommandPool(vkBackends->device, vkBackends->commandPool, NULL);
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

    LvnPlatformData platformData = {0};
    platformData.ndh = createInfo->nativeDisplayHandle;
    platformData.nwh = createInfo->nativeWindowHandle;

    VkSurfaceKHR vkSurface;
    if (lvn_createPlatformSurface(vkBackends, &vkSurface, &platformData) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create VkSurfaceKHR for surface %p", surface);
        return Lvn_Result_Failure;
    }

    surface->surface = vkSurface;
    return Lvn_Result_Success;
}

void lvnImplVkDestroySurface(LvnSurface* surface)
{
    LVN_ASSERT(surface, "surface cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) surface->graphicsctx->implData;

    vkBackends->deviceWaitIdle(vkBackends->device);

    VkSurfaceKHR vkSurface = (VkSurfaceKHR) surface->surface;
    vkBackends->destroySurfaceKHR(vkBackends->instance, vkSurface, NULL);
    surface->surface = NULL;
}

LvnResult lvnImplVkCreateSwapchain(const LvnGraphicsContext* graphicsctx, LvnSwapchain* swapchain, const LvnSwapchainCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && swapchain && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    VkSurfaceKHR vkSurface = (VkSurfaceKHR) createInfo->surface->surface;

    LvnResult errResult = Lvn_Result_Failure;
    LvnVkSwapchainData* swapchainData = NULL;
    LvnTexture* swapchainImages = NULL;

    LvnVkSwapChainCreateInfo swapchainCreateInfo = {
        .physicalDevice = vkBackends->physicalDevice,
        .surface = vkSurface,
        .surfaceFormat = lvn_getVkFormatEnum(createInfo->surfaceFormat),
        .presentMode = lvn_getVkPresentModeEnum(createInfo->presentMode),
        .queueFamilyIndices = &vkBackends->queueFamilyIndices,
        .width = createInfo->width,
        .height = createInfo->height,
        .minImageCount = createInfo->minImageCount,
    };

    swapchainData = (LvnVkSwapchainData*) lvn_calloc(sizeof(LvnVkSwapchainData));
    if (!swapchainData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to allocate memory for swapchain data in swapchain %p", swapchain);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup_swapchaindata;
    }

    if (lvn_createSwapChainData(vkBackends, swapchainData, &swapchainCreateInfo) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create swapchain data for swapchain %p", swapchain);
        errResult = Lvn_Result_Failure;
        goto fail_cleanup;
    }

    swapchainImages = (LvnTexture*) lvn_calloc(swapchainData->swapchainImageCount * sizeof(LvnTexture));
    if (!swapchainImages)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to allocate memory for swapchain image views in swapchain %p", swapchain);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
    {
        swapchainImages[i].imageHandle = swapchainData->swapchainImages[i];
        swapchainImages[i].imageViewHandle = swapchainData->swapchainImageViews[i];
    }

    swapchain->swapchainData = swapchainData;
    swapchain->pSwapchainImages = swapchainImages;
    swapchain->swapchainImageCount = swapchainData->swapchainImageCount;
    swapchain->swapchainColorFormat = lvn_getLvnFormatEnum(swapchainData->swapchainFormat);
    swapchain->extent.width = swapchainData->swapchainExtent.width;
    swapchain->extent.height = swapchainData->swapchainExtent.height;

    return Lvn_Result_Success;

fail_cleanup:
    lvn_free(swapchainImages);
    vkBackends->destroySwapchainKHR(vkBackends->device, swapchainData->swapchain, NULL);
    for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
        vkBackends->destroyImageView(vkBackends->device, swapchainData->swapchainImageViews[i], NULL);
    lvn_free(swapchainData->swapchainImageViews);
    lvn_free(swapchainData->swapchainImages);
fail_cleanup_swapchaindata:
    lvn_free(swapchainData);
    return errResult;
}

void lvnImplVkDestroySwapchain(LvnSwapchain* swapchain)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) swapchain->graphicsctx->implData;
    LvnVkSwapchainData* swapchainData = (LvnVkSwapchainData*) swapchain->swapchainData;

    vkBackends->deviceWaitIdle(vkBackends->device);

    // swapchain image views
    for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
        vkBackends->destroyImageView(vkBackends->device, swapchainData->swapchainImageViews[i], NULL);
    lvn_free(swapchainData->swapchainImageViews);

    // swapchain images
    lvn_free(swapchainData->swapchainImages);

    // swapchain
    vkBackends->destroySwapchainKHR(vkBackends->device, swapchainData->swapchain, NULL);

    // swapchain data struct
    lvn_free(swapchain->swapchainData);
    lvn_free(swapchain->pSwapchainImages);
    swapchain->swapchainData = NULL;
    swapchain->pSwapchainImages = NULL;
}

LvnResult lvnImplVkCreateRenderPass(const LvnGraphicsContext* graphicsctx, LvnRenderPass* renderpass, const LvnRenderPassCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && renderpass && createInfo, "graphicsctx, renderpass, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    LvnResult result = Lvn_Result_Failure;
    VkAttachmentDescription* attachments = NULL;
    VkAttachmentReference* colorAttachmentRefs = NULL;
    VkAttachmentReference* resolveAttachmentRefs = NULL;

    uint32_t attachmentCount = createInfo->colorAttachmentCount;
    uint32_t resolveCount = 0;
    for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
    {
        if (createInfo->pColorAttachments[i].resolveAttachment)
            resolveCount++;
    }
    attachmentCount += resolveCount;
    if (createInfo->depthStencilAttachment)
        attachmentCount++;

    // create array for all attachments, reset attachmentCount to zero for indexing
    attachments = (VkAttachmentDescription*) lvn_calloc(attachmentCount * sizeof(VkAttachmentDescription));
    if (!attachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to allocate memory for (VkAttachmentDescription) attachments array");
        result = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }
    attachmentCount = 0;

    if (createInfo->colorAttachmentCount > 0)
    {
        colorAttachmentRefs = (VkAttachmentReference*) lvn_calloc(createInfo->colorAttachmentCount * sizeof(VkAttachmentReference));
        if (!colorAttachmentRefs)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to allocate memory for (VkAttachmentReference) colorAttachmentRefs array");
            result = Lvn_Result_OutOfMemory;
            goto fail_cleanup;
        }
    }
    if (resolveCount > 0)
    {
        resolveAttachmentRefs = (VkAttachmentReference*) lvn_calloc(createInfo->colorAttachmentCount * sizeof(VkAttachmentReference));
        if (!resolveAttachmentRefs)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to allocate memory for (VkAttachmentReference) resolveAttachmentRefs array");
            result = Lvn_Result_OutOfMemory;
            goto fail_cleanup;
        }
    }

    // color & resolve attachments
    for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
    {
        const LvnColorAttachment* colorAttachment = &createInfo->pColorAttachments[i];

        attachments[attachmentCount].format = lvn_getVkFormatEnum(colorAttachment->format);
        attachments[attachmentCount].samples = lvn_getVkSampleCountFlagEnum(colorAttachment->samples);
        attachments[attachmentCount].loadOp = lvn_getVkAttackmentLoadOpEnum(colorAttachment->loadOp);
        attachments[attachmentCount].storeOp = lvn_getVkAttackmentStoreOpEnum(colorAttachment->storeOp);
        attachments[attachmentCount].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[attachmentCount].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[attachmentCount].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[attachmentCount].finalLayout = lvn_getVkImageLayoutEnumUsage(colorAttachment->usage);

        colorAttachmentRefs[i].attachment = attachmentCount;
        colorAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        attachmentCount++;

        if (colorAttachment->resolveAttachment)
        {
            const LvnResolveAttachment* resolveAttachment = colorAttachment->resolveAttachment;
            attachments[attachmentCount].format = lvn_getVkFormatEnum(resolveAttachment->format);
            attachments[attachmentCount].samples = VK_SAMPLE_COUNT_1_BIT;
            attachments[attachmentCount].loadOp = lvn_getVkAttackmentLoadOpEnum(resolveAttachment->loadOp);
            attachments[attachmentCount].storeOp = lvn_getVkAttackmentStoreOpEnum(resolveAttachment->storeOp);
            attachments[attachmentCount].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachments[attachmentCount].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachments[attachmentCount].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachments[attachmentCount].finalLayout = lvn_getVkImageLayoutEnumUsage(resolveAttachment->usage);

            resolveAttachmentRefs[i].attachment = attachmentCount;
            resolveAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            attachmentCount++;
        }
    }

    // depth attachment
    VkAttachmentReference depthStencilRef = {
        .attachment = VK_ATTACHMENT_UNUSED,
    };

    if (createInfo->depthStencilAttachment)
    {
        const LvnDepthStencilAttachment* depthStencilAttachment = createInfo->depthStencilAttachment;
        attachments[attachmentCount].format = lvn_getVkFormatEnum(depthStencilAttachment->format);
        attachments[attachmentCount].samples = lvn_getVkSampleCountFlagEnum(depthStencilAttachment->samples);
        attachments[attachmentCount].loadOp = lvn_getVkAttackmentLoadOpEnum(depthStencilAttachment->loadOp);
        attachments[attachmentCount].storeOp = lvn_getVkAttackmentStoreOpEnum(depthStencilAttachment->storeOp);
        attachments[attachmentCount].stencilLoadOp = lvn_getVkAttackmentLoadOpEnum(depthStencilAttachment->stencilLoadOp);
        attachments[attachmentCount].stencilStoreOp = lvn_getVkAttackmentStoreOpEnum(depthStencilAttachment->stencilStoreOp);
        attachments[attachmentCount].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[attachmentCount].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        depthStencilRef.attachment = attachmentCount;
        depthStencilRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        attachmentCount++;
    }

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = createInfo->colorAttachmentCount,
        .pColorAttachments = colorAttachmentRefs,
        .pResolveAttachments = resolveAttachmentRefs,
        .pDepthStencilAttachment = (createInfo->depthStencilAttachment) ? &depthStencilRef : NULL,
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };

    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = attachmentCount,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    VkRenderPass vkRenderPass;
    if (vkBackends->createRenderPass(vkBackends->device, &renderPassInfo, NULL, &vkRenderPass) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create render pass");
        goto fail_cleanup;
    }

    renderpass->renderpass = vkRenderPass;

    lvn_free(resolveAttachmentRefs);
    lvn_free(colorAttachmentRefs);
    lvn_free(attachments);

    return Lvn_Result_Success;

fail_cleanup:
    lvn_free(resolveAttachmentRefs);
    lvn_free(colorAttachmentRefs);
    lvn_free(attachments);
    return result;
}

void lvnImplVkDestroyRenderPass(LvnRenderPass* renderpass)
{
    LVN_ASSERT(renderpass, "renderpass cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) renderpass->graphicsctx->implData;
    VkRenderPass vkRenderPass = (VkRenderPass) renderpass->renderpass;

    vkBackends->deviceWaitIdle(vkBackends->device);

    vkBackends->destroyRenderPass(vkBackends->device, vkRenderPass, NULL);
    renderpass->renderpass = NULL;
}

LvnResult lvnImplVkCreateFramebuffer(const LvnGraphicsContext* graphicsctx, LvnFramebuffer* framebuffer, const LvnFramebufferCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && framebuffer && createInfo, "graphicsctx, framebuffer, and createInfo cannot be null");

    LvnVulkanBackends* vkBackends = (LvnVulkanBackends*) graphicsctx->implData;
    VkRenderPass vkRenderPass = (VkRenderPass) createInfo->renderPass->renderpass;
    LvnResult result = Lvn_Result_Failure;

    vkBackends->deviceWaitIdle(vkBackends->device);

    VkImageView* attachments = NULL;

    uint32_t attachmentCount = createInfo->colorAttachmentCount
        + (createInfo->pResolveAttachments ? createInfo->colorAttachmentCount : 0)
        + (createInfo->depthStencilAttachment ? 1 : 0);

    if (attachmentCount > 0)
    {
        attachments = (VkImageView*) lvn_calloc(attachmentCount * sizeof(VkImageView));
        if (!attachments)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to allocate memory for (VkImageView) attachments array");
            result = Lvn_Result_OutOfMemory;
            goto fail_cleanup;
        }

        uint32_t attachmentIndex = 0;
        for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
        {
            attachments[attachmentIndex++] = createInfo->pColorAttachments[i]->imageViewHandle;
            if (createInfo->pResolveAttachments)
                attachments[attachmentIndex++] = createInfo->pResolveAttachments[i]->imageViewHandle;
        }

        if (createInfo->depthStencilAttachment)
            attachments[attachmentIndex++] = createInfo->depthStencilAttachment->imageViewHandle;
    }

    VkFramebufferCreateInfo framebufferInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = vkRenderPass,
        .attachmentCount = attachmentCount,
        .pAttachments = attachments,
        .width = createInfo->width,
        .height = createInfo->height,
        .layers = 1,
    };

    VkFramebuffer vkFramebuffer;
    if (vkBackends->createFramebuffer(vkBackends->device, &framebufferInfo, NULL, &vkFramebuffer) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create framebuffer");
        goto fail_cleanup;
    }

    framebuffer->framebufferHandle = vkFramebuffer;

    lvn_free(attachments);

    return Lvn_Result_Success;

fail_cleanup:
    lvn_free(attachments);
    return result;
}

void lvnImplVkDestroyFramebuffer(LvnFramebuffer* framebuffer)
{
    LVN_ASSERT(framebuffer, "framebuffer cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) framebuffer->graphicsctx->implData;
    VkFramebuffer vkFramebuffer = (VkFramebuffer) framebuffer->framebufferHandle;

    vkBackends->deviceWaitIdle(vkBackends->device);

    vkBackends->destroyFramebuffer(vkBackends->device, vkFramebuffer, NULL);
    framebuffer->framebufferHandle = NULL;
}

LvnResult lvnImplVkCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && shader && createInfo, "graphicsctx, shader, and createInfo cannot be null");

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

    vkBackends->deviceWaitIdle(vkBackends->device);

    VkShaderModule shaderModule = (VkShaderModule) shader->shader;
    vkBackends->destroyShaderModule(vkBackends->device, shaderModule, NULL);
    shader->shader = NULL;
}

LvnResult lvnImplVkCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && pipeline && createInfo, "graphicsctx, pipeline, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline vkPipeline = VK_NULL_HANDLE;

    VkPipelineShaderStageCreateInfo* shaderStages = NULL;
    VkVertexInputBindingDescription* bindingDescriptions = NULL;
    VkVertexInputAttributeDescription* vertexAttributes = NULL;
    VkDescriptorSetLayout* descriptorLayouts = NULL;
    VkFormat* colorAttachmentFormats = NULL;
    VkPipelineColorBlendAttachmentState* colorBlendAttachments = NULL;

    // shader stages
    shaderStages = (VkPipelineShaderStageCreateInfo*)
        lvn_calloc(createInfo->stageCount * sizeof(VkPipelineShaderStageCreateInfo));
    if (createInfo->stageCount && !shaderStages)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "malloc failure on creating shaderStages array (VkPipelineShaderStageCreateInfo*)");
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->stageCount; i++)
    {
        VkPipelineShaderStageCreateInfo stageCreateInfo = {0};
        stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageCreateInfo.stage = lvn_getVkShaderStageEnum(createInfo->pStages[i].stage);
        stageCreateInfo.module = (VkShaderModule) createInfo->pStages[i].shader->shader;
        stageCreateInfo.pName = createInfo->pStages[i].entryPoint;
        shaderStages[i] = stageCreateInfo;
    }

    // vertex binding descriptions
    bindingDescriptions = (VkVertexInputBindingDescription*)
        lvn_calloc(createInfo->vertexBindingDescriptionCount * sizeof(VkVertexInputBindingDescription));
    if (createInfo->vertexBindingDescriptionCount && !bindingDescriptions)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "malloc failure on creating bindingDescriptions array (VkVertexInputBindingDescription*)");
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->vertexBindingDescriptionCount; i++)
    {
        VkVertexInputBindingDescription bindingDescription = {0};
        bindingDescription.binding = createInfo->pVertexBindingDescriptions[i].binding;
        bindingDescription.stride = createInfo->pVertexBindingDescriptions[i].stride;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        bindingDescriptions[i] = bindingDescription;
    }

    // vertex attributes
    vertexAttributes = (VkVertexInputAttributeDescription*)
        lvn_calloc(createInfo->vertexAttributeCount * sizeof(VkVertexInputAttributeDescription));
    if (createInfo->vertexAttributeCount && !vertexAttributes)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "malloc failure on creating vertexAttributes array (VkVertexInputAttributeDescription*)");
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->vertexAttributeCount; i++)
    {
        VkVertexInputAttributeDescription attributeDescription = {0};
        attributeDescription.binding = createInfo->pVertexAttributes[i].binding;
        attributeDescription.location = createInfo->pVertexAttributes[i].layout;
        attributeDescription.format = lvn_getVkVertexAttributeFormatEnum(createInfo->pVertexAttributes[i].format);
        attributeDescription.offset = createInfo->pVertexAttributes[i].offset;

        vertexAttributes[i] = attributeDescription;
    }

    // send binding descriptions and attributes to pipeline
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    if (createInfo->pVertexBindingDescriptions && createInfo->vertexBindingDescriptionCount > 0)
    {
        vertexInputInfo.vertexBindingDescriptionCount = createInfo->vertexBindingDescriptionCount;
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions;
    }

    if (createInfo->pVertexAttributes && createInfo->vertexAttributeCount > 0)
    {
        vertexInputInfo.vertexAttributeDescriptionCount = createInfo->vertexAttributeCount;
        vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes;
    }

    // descriptor layouts
    descriptorLayouts = (VkDescriptorSetLayout*)
        lvn_calloc(createInfo->descriptorLayoutCount * sizeof(VkDescriptorSetLayout));

    if (createInfo->descriptorLayoutCount && !descriptorLayouts)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "malloc failure on creating descriptorLayouts array (VkDescriptorSetLayout*)");
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->descriptorLayoutCount; i++)
    {
        VkDescriptorSetLayout descriptorLayout = (VkDescriptorSetLayout) createInfo->pDescriptorLayouts[i]->descriptorLayout;
        descriptorLayouts[i] = descriptorLayout;
    }

    // pipeline fixed functions
    const LvnPipelineFixedFunctions* pipelineFixedFunctions = createInfo->pipelineFixedFunctions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = lvn_getVkTopologyTypeEnum(pipelineFixedFunctions->inputAssembly.topology);
    inputAssembly.primitiveRestartEnable = pipelineFixedFunctions->inputAssembly.primitiveRestartEnable;

    VkDynamicState dynamicStates[5];
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
    uint32_t dynamicStatesCount = 2;

    if (pipelineFixedFunctions->depthstencil.enableStencil)
    {
        dynamicStates[2] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;
        dynamicStates[3] = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
        dynamicStates[4] = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
        dynamicStatesCount = 5;
    }

    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStates;
    dynamicState.dynamicStateCount = dynamicStatesCount;

    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = pipelineFixedFunctions->rasterizer.depthClampEnable;
    rasterizer.rasterizerDiscardEnable = pipelineFixedFunctions->rasterizer.rasterizerDiscardEnable;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = pipelineFixedFunctions->rasterizer.lineWidth;
    rasterizer.cullMode = lvn_getVkCullModeFlagEnum(pipelineFixedFunctions->rasterizer.cullMode);
    rasterizer.frontFace = lvn_getVkCullFrontFaceEnum(pipelineFixedFunctions->rasterizer.frontFace);
    rasterizer.depthBiasEnable = pipelineFixedFunctions->rasterizer.depthBiasEnable;
    rasterizer.depthBiasConstantFactor = pipelineFixedFunctions->rasterizer.depthBiasConstantFactor;
    rasterizer.depthBiasClamp = pipelineFixedFunctions->rasterizer.depthBiasClamp;
    rasterizer.depthBiasSlopeFactor = pipelineFixedFunctions->rasterizer.depthBiasSlopeFactor;

    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = pipelineFixedFunctions->multisampling.sampleShadingEnable;
    multisampling.rasterizationSamples = lvn_getVkSampleCountFlagEnum(pipelineFixedFunctions->multisampling.rasterizationSamples);
    multisampling.minSampleShading = pipelineFixedFunctions->multisampling.minSampleShading;
    multisampling.pSampleMask = pipelineFixedFunctions->multisampling.sampleMask;
    multisampling.alphaToCoverageEnable = pipelineFixedFunctions->multisampling.alphaToCoverageEnable;
    multisampling.alphaToOneEnable = pipelineFixedFunctions->multisampling.alphaToOneEnable;

    // if color blend attachments is 0, we automatically add a default color blend attachment
    uint32_t colorBlendAttachmentCount = (pipelineFixedFunctions->colorBlend.colorBlendAttachmentCount == 0)
        ? 1
        : pipelineFixedFunctions->colorBlend.colorBlendAttachmentCount;

    colorBlendAttachments = (VkPipelineColorBlendAttachmentState*)
        lvn_calloc(colorBlendAttachmentCount * sizeof(VkPipelineColorBlendAttachmentState));

    if (colorBlendAttachmentCount && !colorBlendAttachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "malloc failure on creating colorBlendAttachments array (VkPipelineColorBlendAttachmentState*)");
        goto fail_cleanup;
    }

    if (pipelineFixedFunctions->colorBlend.colorBlendAttachmentCount == 0)
    {
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachments[0] = colorBlendAttachment;
    }
    else
    {
        for (uint32_t i = 0; i < colorBlendAttachmentCount; i++)
        {
            LvnPipelineColorBlendAttachment attachment = pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i];

            VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
            colorBlendAttachment.colorWriteMask = lvn_getVkColorComponentsFlagEnum(attachment.colorWriteMask);
            colorBlendAttachment.blendEnable = attachment.blendEnable;
            colorBlendAttachment.srcColorBlendFactor = lvn_getVkBlendFactorEnum(attachment.srcColorBlendFactor);
            colorBlendAttachment.dstColorBlendFactor = lvn_getVkBlendFactorEnum(attachment.dstColorBlendFactor);
            colorBlendAttachment.colorBlendOp = lvn_getVkBlendOperationEnum(attachment.colorBlendOp);
            colorBlendAttachment.srcAlphaBlendFactor = lvn_getVkBlendFactorEnum(attachment.srcAlphaBlendFactor);
            colorBlendAttachment.dstAlphaBlendFactor = lvn_getVkBlendFactorEnum(attachment.dstAlphaBlendFactor);
            colorBlendAttachment.alphaBlendOp = lvn_getVkBlendOperationEnum(attachment.alphaBlendOp);

            colorBlendAttachments[i] = colorBlendAttachment;
        }
    }

    VkPipelineColorBlendStateCreateInfo colorBlending = {0};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = pipelineFixedFunctions->colorBlend.logicOpEnable;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.pAttachments = colorBlendAttachments;
    colorBlending.attachmentCount = colorBlendAttachmentCount;
    colorBlending.blendConstants[0] = pipelineFixedFunctions->colorBlend.blendConstants[0];
    colorBlending.blendConstants[1] = pipelineFixedFunctions->colorBlend.blendConstants[1];
    colorBlending.blendConstants[2] = pipelineFixedFunctions->colorBlend.blendConstants[2];
    colorBlending.blendConstants[3] = pipelineFixedFunctions->colorBlend.blendConstants[3];

    VkPipelineDepthStencilStateCreateInfo depthStencil = {0};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = pipelineFixedFunctions->depthstencil.enableDepth;
    depthStencil.depthWriteEnable = pipelineFixedFunctions->depthstencil.enableDepth;
    depthStencil.depthCompareOp = lvn_getVkCompareOpEnum(pipelineFixedFunctions->depthstencil.depthOpCompare);
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;
    depthStencil.stencilTestEnable = pipelineFixedFunctions->depthstencil.enableStencil;
    depthStencil.back.compareMask = pipelineFixedFunctions->depthstencil.stencil.compareMask;
    depthStencil.back.writeMask = pipelineFixedFunctions->depthstencil.stencil.writeMask;
    depthStencil.back.reference = pipelineFixedFunctions->depthstencil.stencil.reference;
    depthStencil.back.compareOp = lvn_getVkCompareOpEnum(pipelineFixedFunctions->depthstencil.stencil.compareOp);
    depthStencil.back.depthFailOp = lvn_getVkStencilOpEnum(pipelineFixedFunctions->depthstencil.stencil.depthFailOp);
    depthStencil.back.failOp = lvn_getVkStencilOpEnum(pipelineFixedFunctions->depthstencil.stencil.failOp);
    depthStencil.back.passOp = lvn_getVkStencilOpEnum(pipelineFixedFunctions->depthstencil.stencil.passOp);
    depthStencil.front = depthStencil.back;

    // pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = NULL;

    if (createInfo->descriptorLayoutCount != 0)
    {
        pipelineLayoutInfo.setLayoutCount = createInfo->descriptorLayoutCount;
        pipelineLayoutInfo.pSetLayouts = descriptorLayouts;
    }
    else
    {
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = NULL;
    }

    if (vkBackends->createPipelineLayout(vkBackends->device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create pipeline layout for pipeline %p", pipeline);
        goto fail_cleanup;
    }

    VkRenderPass renderPass = (VkRenderPass) createInfo->renderPass->renderpass;

    // pipeline create info
    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = createInfo->stageCount;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;
    pipelineInfo.renderPass = renderPass;

    if (vkBackends->createGraphicsPipelines(vkBackends->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &vkPipeline) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create graphics pipeline for pipeline %p", pipeline);
        goto fail_cleanup;
    }

    pipeline->pipelineHandle = vkPipeline;
    pipeline->pipelineLayoutHandle = pipelineLayout;

    lvn_free(colorBlendAttachments);
    lvn_free(colorAttachmentFormats);
    lvn_free(descriptorLayouts);
    lvn_free(vertexAttributes);
    lvn_free(bindingDescriptions);
    lvn_free(shaderStages);

    return Lvn_Result_Success;

fail_cleanup:
    vkBackends->destroyPipeline(vkBackends->device, vkPipeline, NULL);
    vkBackends->destroyPipelineLayout(vkBackends->device, pipelineLayout, NULL);
    lvn_free(colorBlendAttachments);
    lvn_free(colorAttachmentFormats);
    lvn_free(descriptorLayouts);
    lvn_free(vertexAttributes);
    lvn_free(bindingDescriptions);
    lvn_free(shaderStages);
    return Lvn_Result_Failure;
}

void lvnImplVkDestroyPipeline(LvnPipeline* pipeline)
{
    LVN_ASSERT(pipeline, "pipeline cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) pipeline->graphicsctx->implData;

    vkBackends->deviceWaitIdle(vkBackends->device);

    VkPipeline vkPipeline = (VkPipeline) pipeline->pipelineHandle;
    VkPipelineLayout pipelineLayout = (VkPipelineLayout) pipeline->pipelineLayoutHandle;

    vkBackends->destroyPipeline(vkBackends->device, vkPipeline, NULL);
    vkBackends->destroyPipelineLayout(vkBackends->device, pipelineLayout, NULL);
}

LvnResult lvnImplVkCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence* fence)
{
    LVN_ASSERT(graphicsctx && fence, "graphicsctx and fence cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    VkFence vkFence;
    if (vkBackends->createFence(vkBackends->device, &fenceInfo, NULL, &vkFence) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create VkFence");
        return Lvn_Result_Failure;
    }

    fence->fenceHandle = vkFence;
    return Lvn_Result_Success;
}

void lvnImplVkDestroyFence(LvnFence* fence)
{
    LVN_ASSERT(fence, "fence cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) fence->graphicsctx->implData;
    vkBackends->deviceWaitIdle(vkBackends->device);
    VkFence vkFence = (VkFence) fence->fenceHandle;
    vkBackends->destroyFence(vkBackends->device, vkFence, NULL);
}

LvnResult lvnImplVkCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore* semaphore)
{
    LVN_ASSERT(graphicsctx && semaphore, "graphicsctx and semaphore cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkSemaphore vkSemaphore;
    if (vkBackends->createSemaphore(vkBackends->device, &semaphoreInfo, NULL, &vkSemaphore) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create VkSemaphore");
        return Lvn_Result_Failure;
    }

    semaphore->semaphoreHandle = vkSemaphore;
    return Lvn_Result_Success;
}

void lvnImplVkDestroySemaphore(LvnSemaphore* semaphore)
{
    LVN_ASSERT(semaphore, "semaphore cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) semaphore->graphicsctx->implData;
    vkBackends->deviceWaitIdle(vkBackends->device);
    VkSemaphore vkSemaphore = (VkSemaphore) semaphore->semaphoreHandle;
    vkBackends->destroySemaphore(vkBackends->device, vkSemaphore, NULL);
}

LvnResult lvnImplVksCreateBuffer(const LvnGraphicsContext* graphicsctx, LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && buffer && createInfo, "graphicsctx, buffer, and createInfo cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    VkDeviceSize bufferSize = createInfo->size;

    VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (createInfo->type & Lvn_BufferTypeFlag_Vertex)
        usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (createInfo->type & Lvn_BufferTypeFlag_Index)
        usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (createInfo->type & Lvn_BufferTypeFlag_Uniform)
        usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (createInfo->type & Lvn_BufferTypeFlag_Storage)
        usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    // if buffer is static, transfer and store memory on gpu
    if (createInfo->usage == Lvn_BufferUsage_Static)
    {
        VkBuffer stagingBuffer;
        VmaAllocation stagingMemory;

        VkBuffer vkBuffer;
        VmaAllocation bufferMemory;

        // create staging buffer to pass vertex data into
        lvn_createBuffer(vkBackends, &stagingBuffer, &stagingMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

        if (createInfo->data)
        {
            void* data;
            vmaMapMemory(vkBackends->vmaAllocator, stagingMemory, &data);
            memcpy(data, createInfo->data, bufferSize);
            vmaUnmapMemory(vkBackends->vmaAllocator, stagingMemory);
        }

        // create the main buffer to be used
        lvn_createBuffer(vkBackends, &vkBuffer, &bufferMemory, bufferSize, usageFlags, VMA_MEMORY_USAGE_GPU_ONLY);
        lvn_copyBuffer(vkBackends, stagingBuffer, vkBuffer, bufferSize, 0, 0);

        vkBackends->destroyBuffer(vkBackends->device, stagingBuffer, NULL);
        vmaFreeMemory(vkBackends->vmaAllocator, stagingMemory);

        buffer->buffer = vkBuffer;
        buffer->bufferMemory = bufferMemory;
    }
    else // dynamic buffers will store memory on cpu
    {
        VkBuffer vkBuffer;
        VmaAllocation bufferMemory;

        lvn_createBuffer(vkBackends, &vkBuffer, &bufferMemory, bufferSize, usageFlags, VMA_MEMORY_USAGE_CPU_ONLY);

        vmaMapMemory(vkBackends->vmaAllocator, bufferMemory, &buffer->bufferMap);
        if (createInfo->data)
            memcpy(buffer->bufferMap, createInfo->data, bufferSize);

        buffer->buffer = vkBuffer;
        buffer->bufferMemory = bufferMemory;
    }

    buffer->type = createInfo->type;
    buffer->usage = createInfo->usage;
    buffer->size = createInfo->size;

    return Lvn_Result_Success;
}

void lvnImplVksDestroyBuffer(LvnBuffer* buffer)
{
    LVN_ASSERT(buffer, "buffer cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) buffer->graphicsctx->implData;
    vkBackends->deviceWaitIdle(vkBackends->device);

    VkBuffer vkBuffer = (VkBuffer) buffer->buffer;
    VmaAllocation bufferMemory = (VmaAllocation) buffer->bufferMemory;

    if (buffer->usage != Lvn_BufferUsage_Static)
        vmaUnmapMemory(vkBackends->vmaAllocator, bufferMemory);

    vkBackends->destroyBuffer(vkBackends->device, vkBuffer, NULL);
    vmaFreeMemory(vkBackends->vmaAllocator, bufferMemory);
}

LvnResult lvnImplVksCreateSampler(const LvnGraphicsContext* graphicsctx, LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && sampler && createInfo, "graphicsctx, sampler, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .minFilter = lvn_getVkTextureFilterEnum(createInfo->minFilter),
        .magFilter = lvn_getVkTextureFilterEnum(createInfo->magFilter),
        .addressModeU = lvn_getVkTextureModeEnum(createInfo->wrapS),
        .addressModeV = lvn_getVkTextureModeEnum(createInfo->wrapT),
        .addressModeW = lvn_getVkTextureModeEnum(createInfo->wrapR),
    };

    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkBackends->getPhysicalDeviceFeatures(vkBackends->physicalDevice, &physicalDeviceFeatures);

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkBackends->getPhysicalDeviceProperties(vkBackends->physicalDevice, &physicalDeviceProperties);

    if (physicalDeviceFeatures.samplerAnisotropy)
    {
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
    }
    else
    {
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
    }

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkSampler textureSampler;

    if (vkBackends->createSampler(vkBackends->device, &samplerInfo, NULL, &textureSampler) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create texture sampler <VkSampler> (%p)",
                      textureSampler);
        return Lvn_Result_Failure;
    }

    sampler->samplerHandle = textureSampler;

    return Lvn_Result_Success;
}

void lvnImplVksDestroySampler(LvnSampler* sampler)
{
    LVN_ASSERT(sampler, "sampler cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) sampler->graphicsctx->implData;
    vkBackends->deviceWaitIdle(vkBackends->device);
    VkSampler textureSampler = (VkSampler) sampler->samplerHandle;
    vkBackends->destroySampler(vkBackends->device, textureSampler, NULL);
}

LvnResult lvnImplVksCreateTexture(const LvnGraphicsContext* graphicsctx, LvnTexture* texture, const LvnTextureCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && texture && createInfo, "graphicsctx, texture, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    LvnResult result = Lvn_Result_Failure;

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VmaAllocation stagingBufferMemory = VK_NULL_HANDLE;
    VkImage textureImage = VK_NULL_HANDLE;
    VmaAllocation textureImageMemory = VK_NULL_HANDLE;

    VkDeviceSize imageSize = createInfo->image->width * createInfo->image->height * createInfo->image->channels;
    if (lvn_createBuffer(vkBackends, &stagingBuffer, &stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY) != Lvn_Result_Success)
        goto fail_cleanup;

    void* data;
    vmaMapMemory(vkBackends->vmaAllocator, stagingBufferMemory, &data);
    memcpy(data, createInfo->image->data, imageSize);
    vmaUnmapMemory(vkBackends->vmaAllocator, stagingBufferMemory);

    VkFormat format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB;
    switch (createInfo->image->channels)
    {
        case 1: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8_UNORM : VK_FORMAT_R8_SRGB; break; }
        case 2: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_SRGB; break; }
        case 4: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB; break; }
    }

    // create texture image
    if (lvn_createImage(vkBackends,
        &textureImage,
        &textureImageMemory,
        createInfo->image->width,
        createInfo->image->height,
        format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_SAMPLE_COUNT_1_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create texture image <VkImage> for texture (%p)",
                      texture);
        goto fail_cleanup;
    }

    // transition buffer to image
    lvn_transitionImageLayout(vkBackends, textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
    lvn_copyBufferToImage(vkBackends, stagingBuffer, textureImage, createInfo->image->width, createInfo->image->height, 1);
    lvn_transitionImageLayout(vkBackends, textureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

    // texture image view
    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = textureImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1,
    };

    VkImageView imageView;
    if (vkBackends->createImageView(vkBackends->device, &viewInfo, NULL, &imageView) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create texture image view <VkImageView> for texture (%p)",
                      texture);
        goto fail_cleanup;
    }

    texture->imageHandle = textureImage;
    texture->imageMemoryHandle = textureImageMemory;
    texture->imageViewHandle = imageView;

    vkBackends->destroyBuffer(vkBackends->device, stagingBuffer, NULL);
    vmaFreeMemory(vkBackends->vmaAllocator, stagingBufferMemory);

    return Lvn_Result_Success;

fail_cleanup:
    vkBackends->destroyImage(vkBackends->device, textureImage, NULL);
    vkBackends->destroyBuffer(vkBackends->device, stagingBuffer, NULL);
    vmaFreeMemory(vkBackends->vmaAllocator, stagingBufferMemory);
    vmaFreeMemory(vkBackends->vmaAllocator, textureImageMemory);
    return result;
}

void lvnImplVksDestroyTexture(LvnTexture* texture)
{
    LVN_ASSERT(texture, "texture cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) texture->graphicsctx->implData;

    vkBackends->deviceWaitIdle(vkBackends->device);

    VkImage image = (VkImage) texture->imageHandle;
    VmaAllocation imageMemory = (VmaAllocation) texture->imageMemoryHandle;
    VkImageView imageView = (VkImageView) texture->imageViewHandle;

    vkBackends->destroyImage(vkBackends->device, image, NULL);
    vmaFreeMemory(vkBackends->vmaAllocator, imageMemory);
    vkBackends->destroyImageView(vkBackends->device, imageView, NULL);
}

LvnResult lvnImplVkAllocateCommandBuffers(const LvnGraphicsContext* graphicsctx, const LvnCommandBufferAllocInfo* allocInfo, LvnCommandBuffer** pCommandBuffers)
{
    LVN_ASSERT(graphicsctx && allocInfo && pCommandBuffers, "graphicsctx, allocInfo, and pCommandBuffers cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkCommandBufferAllocateInfo cmdBufferAllocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vkBackends->commandPool,
        .level = lvn_getVkCommandBufferLevelEnum(allocInfo->level),
        .commandBufferCount = allocInfo->count,
    };

    VkCommandBuffer* commandBuffers = lvn_calloc(allocInfo->count * sizeof(VkCommandBuffer));
    if (vkBackends->allocateCommandBuffers(vkBackends->device, &cmdBufferAllocInfo, commandBuffers) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to allocate command buffer");
        return Lvn_Result_Failure;
    }

    for (uint32_t i = 0; i < allocInfo->count; i++)
        pCommandBuffers[i]->commandbuffer = commandBuffers[i];

    lvn_free(commandBuffers);
    return Lvn_Result_Success;
}

void lvnImplVkSurfaceGetSupportedFormats(const LvnSurface* surface, uint32_t* formatCount, LvnFormat* pSurfaceFormats)
{
    LVN_ASSERT(surface && formatCount, "surface and formatCount cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) surface->graphicsctx->implData;
    VkSurfaceKHR vkSurface = (VkSurfaceKHR) surface->surface;

    uint32_t vkFormatCount = 0;
    vkBackends->getPhysicalDeviceSurfaceFormatsKHR(vkBackends->physicalDevice, vkSurface, &vkFormatCount, NULL);

    if (!vkFormatCount)
    {
        *formatCount = 0;
        return;
    }

    VkSurfaceFormatKHR* formats = (VkSurfaceFormatKHR*) lvn_calloc(vkFormatCount * sizeof(VkSurfaceFormatKHR));
    if (!formats)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger,
                      "[vulkan] failed to allocate temporary format (VkSurfaceFormatKHR) array when querying surface formats");
        return;
    }

    vkBackends->getPhysicalDeviceSurfaceFormatsKHR(vkBackends->physicalDevice, vkSurface, &vkFormatCount, formats);

    uint32_t supportedFormatCount = 0;
    for (uint32_t i = 0; i < vkFormatCount; i++)
    {
        // NOTE: support other colorspace in future?
        if (formats[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            continue;

        LvnFormat format = lvn_getLvnFormatEnum(formats[i].format);
        if (format != Lvn_Format_None)
            supportedFormatCount++;

        if (pSurfaceFormats)
            pSurfaceFormats[i] = format;
    }

    *formatCount = supportedFormatCount;

    lvn_free(formats);
}

void lvnImplVkSurfaceGetSupportedPresentModes(const LvnSurface* surface, uint32_t* presentModeCount, LvnPresentMode* pPresentModes)
{
    LVN_ASSERT(surface && presentModeCount, "surface and presentModeCount cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) surface->graphicsctx->implData;
    VkSurfaceKHR vkSurface = (VkSurfaceKHR) surface->surface;

    uint32_t vkPresentModeCount = 0;
    vkBackends->getPhysicalDeviceSurfacePresentModesKHR(vkBackends->physicalDevice, vkSurface, &vkPresentModeCount, NULL);

    if (!vkPresentModeCount)
    {
        *presentModeCount = 0;
        return;
    }

    VkPresentModeKHR* presentModes = (VkPresentModeKHR*) lvn_calloc(vkPresentModeCount * sizeof(VkPresentModeKHR));
    if (!presentModes)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger,
                      "[vulkan] failed to allocate temporary format (VkPresentModeKHR) array when querying surface present modes");
        return;
    }

    vkBackends->getPhysicalDeviceSurfacePresentModesKHR(vkBackends->physicalDevice, vkSurface, &vkPresentModeCount, presentModes);

    bool fifoFound = false;
    uint32_t supportedPresentModeCount = 0;
    for (uint32_t i = 0; i < vkPresentModeCount; i++)
    {
        LvnPresentMode presentMode = lvn_getLvnPresentModeEnum(presentModes[i]);
        if (presentMode == Lvn_PresentMode_FIFO)
        {
            if (fifoFound)
                continue;
            else
                fifoFound = true;
        }

        supportedPresentModeCount++;

        if (pPresentModes)
            pPresentModes[i] = presentMode;
    }

    *presentModeCount = supportedPresentModeCount;

    lvn_free(presentModes);
}

LvnResult lvnImplVkSwapchainResize(LvnSwapchain* swapchain, uint32_t width, uint32_t height)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) swapchain->graphicsctx->implData;

    vkBackends->deviceWaitIdle(vkBackends->device);

    LvnVkSwapchainData* swapchainData = (LvnVkSwapchainData*) swapchain->swapchainData;
    VkSurfaceKHR surface = (VkSurfaceKHR) swapchainData->surface;

    // destroy swapchain resources
    for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
        vkBackends->destroyImageView(vkBackends->device, swapchainData->swapchainImageViews[i], NULL);
    lvn_free(swapchainData->swapchainImages);
    lvn_free(swapchainData->swapchainImageViews);
    swapchainData->swapchainImages = NULL;
    swapchainData->swapchainImageViews = NULL;

    // set current swapchain to old swapchain
    swapchainData->oldSwapchain = swapchainData->swapchain;
    swapchainData->swapchain = VK_NULL_HANDLE;

    LvnVkSwapChainCreateInfo swapchainCreateInfo = {
        .physicalDevice = vkBackends->physicalDevice,
        .surface = surface,
        .surfaceFormat = swapchainData->swapchainFormat,
        .presentMode = swapchainData->presentMode,
        .queueFamilyIndices = &vkBackends->queueFamilyIndices,
        .width = width,
        .height = height,
        .minImageCount = swapchainData->swapchainImageCount,
    };

    // create new swapchain
    if (lvn_createSwapChainData(vkBackends, swapchainData, &swapchainCreateInfo) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger,
                      "[vulkan] failed to recreate swapchain in LvnSurface %p",
                      surface);
        return Lvn_Result_Failure;
    }

    // destroy old swapchain
    vkBackends->destroySwapchainKHR(vkBackends->device, swapchainData->oldSwapchain, NULL);
    swapchainData->oldSwapchain = VK_NULL_HANDLE;

    if (swapchain->swapchainImageCount != swapchainData->swapchainImageCount)
    {
        swapchain->swapchainImageCount = swapchainData->swapchainImageCount;
        swapchain->pSwapchainImages = lvn_realloc(swapchain->pSwapchainImages, swapchainData->swapchainImageCount * sizeof(LvnTexture*));
    }

    // update image views in LvnSurface struct
    for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
    {
        swapchain->pSwapchainImages[i].imageHandle = swapchainData->swapchainImages[i];
        swapchain->pSwapchainImages[i].imageViewHandle = swapchainData->swapchainImageViews[i];
    }

    // update extent
    swapchain->extent.width = swapchainData->swapchainExtent.width;
    swapchain->extent.height = swapchainData->swapchainExtent.height;

    return Lvn_Result_Success;
}

LvnResult lvnImplVkSwapchainAcquireNextImage(LvnSwapchain* swapchain, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex)
{
    LVN_ASSERT(swapchain && imageIndex, "swapchain and imageIndex cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) swapchain->graphicsctx->implData;
    VkSwapchainKHR vkSwapchain = ((LvnVkSwapchainData*)swapchain->swapchainData)->swapchain;
    VkSemaphore vkSemaphore = (semaphore != NULL) ? (VkSemaphore) semaphore->semaphoreHandle : VK_NULL_HANDLE;
    VkFence vkFence = (fence != NULL) ? (VkFence) fence->fenceHandle : VK_NULL_HANDLE;

    VkResult result = vkBackends->acquireNextImageKHR(vkBackends->device, vkSwapchain, UINT64_MAX, vkSemaphore, vkFence, imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
        return Lvn_Result_OutOfDate;

    return (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) ? Lvn_Result_Success : Lvn_Result_Failure;
}

LvnResult lvnImplVkFenceWait(LvnFence* fence, uint64_t timeout)
{
    LVN_ASSERT(fence, "fence cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) fence->graphicsctx->implData;
    VkFence vkFence = (VkFence) fence->fenceHandle;
    return (vkBackends->waitForFences(vkBackends->device, 1, &vkFence, VK_TRUE, timeout) == VK_SUCCESS)
        ? Lvn_Result_Success
        : Lvn_Result_Failure;
}

LvnResult lvnImplVkFenceReset(LvnFence* fence)
{
    LVN_ASSERT(fence, "fence cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) fence->graphicsctx->implData;
    VkFence vkFence = (VkFence) fence->fenceHandle;
    return (vkBackends->resetFences(vkBackends->device, 1, &vkFence) == VK_SUCCESS)
        ? Lvn_Result_Success
        : Lvn_Result_Failure;
}

void lvnImplVkBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset)
{
    LVN_ASSERT(buffer, "buffer cannot be null");
    memcpy((uint8_t*)buffer->bufferMap + offset, data, size);
}

void lvnImplVkBufferResize(LvnBuffer* buffer, uint64_t size)
{

}

void lvnImplVkBeginCommandBuffer(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;

    VkCommandBufferBeginInfo cmdBuffBeginInfo = {0};
    cmdBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBuffBeginInfo.flags = 0;
    cmdBuffBeginInfo.pInheritanceInfo = NULL;

    if (vkBackends->beginCommandBuffer(cmdBuff, &cmdBuffBeginInfo) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to begin command buffer");
    }
}

void lvnImplVkEndCommandBuffer(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;
    vkBackends->endCommandBuffer(cmdBuff);
}

void lvnImplVkCmdBeginRenderPass(LvnCommandBuffer* commandBuffer, LvnRenderPassBeginInfo* beginInfo)
{
    LVN_ASSERT(commandBuffer && beginInfo, "commandBuffer and beginInfo cannot be null");
    const LvnGraphicsContext* graphicsctx = (const LvnGraphicsContext*) commandBuffer->graphicsctx;
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;
    VkRenderPass renderPass = (VkRenderPass) beginInfo->renderPass->renderpass;
    VkFramebuffer framebuffer = (VkFramebuffer) beginInfo->framebuffer->framebufferHandle;

    VkRect2D renderArea = {
        .offset = { beginInfo->renderArea.offset.x, beginInfo->renderArea.offset.y },
        .extent = { beginInfo->renderArea.extent.width, beginInfo->renderArea.extent.height },
    };

    VkClearValue* clearColors =
        lvn_memArenaAlloc(graphicsctx->frameArena, beginInfo->clearValueCount * sizeof(VkClearValue));

    for (uint32_t i = 0; i < beginInfo->clearValueCount; i++)
        memcpy(&clearColors[i], &beginInfo->pClearValues[i], sizeof(VkClearValue));

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = framebuffer,
        .renderArea = renderArea,
        .clearValueCount = beginInfo->clearValueCount,
        .pClearValues = clearColors,
    };

    vkBackends->cmdBeginRenderPass(cmdBuff, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    lvn_memArenaReset(graphicsctx->frameArena);
}

void lvnImplVkCmdEndRenderPass(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;
    vkBackends->cmdEndRenderPass(cmdBuff);
}

void lvnImplVkCmdBindPipeline(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;
    VkPipeline vkPipeline = (VkPipeline) pipeline->pipelineHandle;

    vkBackends->cmdBindPipeline(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
}

void lvnImplVkCmdBindVertexBuffer(LvnCommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = (const LvnGraphicsContext*) commandBuffer->graphicsctx;
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;

    VkBuffer* buffers = lvn_memArenaAlloc(graphicsctx->frameArena, bindingCount * sizeof(VkBuffer));
    for (uint32_t i = 0; i < bindingCount; i++)
        buffers[i] = (VkBuffer) pBuffers[i]->buffer;

    vkBackends->cmdBindVertexBuffers(cmdBuff, firstBinding, bindingCount, buffers, pOffsets);
}

void lvnImplVkCmdBindIndexBuffer(LvnCommandBuffer* commandBuffer, LvnBuffer* buffer, uint64_t offset)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;

    VkBuffer indexBuffer = (VkBuffer) buffer->buffer;

   vkBackends->cmdBindIndexBuffer(cmdBuff, indexBuffer, offset, VK_INDEX_TYPE_UINT32);
}

void lvnImplVkCmdSetViewport(LvnCommandBuffer* commandBuffer, const LvnViewport* viewport)
{
    LVN_ASSERT(commandBuffer && viewport, "commandBuffer and viewport cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;

    VkViewport viewportInfo = {
        .x        = viewport->x,
        .y        = viewport->y,
        .width    = viewport->width,
        .height   = viewport->height,
        .minDepth = viewport->minDepth,
        .maxDepth = viewport->maxDepth,
    };

    vkBackends->cmdSetViewport(cmdBuff, 0, 1, &viewportInfo);
}

void lvnImplVkCmdSetScissor(LvnCommandBuffer* commandBuffer, const LvnRenderArea* scissor)
{
    LVN_ASSERT(commandBuffer && scissor, "commandBuffer and scissor cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;

    VkRect2D scissorInfo = {
        .extent = { scissor->extent.width, scissor->extent.height },
        .offset = { scissor->offset.x, scissor->offset.y },
    };

    vkBackends->cmdSetScissor(cmdBuff, 0, 1, &scissorInfo);
}

void lvnImplVkCmdDraw(LvnCommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;
    vkBackends->cmdDraw(cmdBuff, vertexCount, instanceCount, firstVertex, firstInstance);
}

void lvnImplVkCmdDrawIndexed(LvnCommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbuffer;
    vkBackends->cmdDrawIndexed(cmdBuff, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

LvnResult lvnImplVkRenderSubmit(const LvnGraphicsContext* graphicsctx, const LvnSubmitInfo* pSubmits, uint32_t submitCount, LvnFence* fence)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    VkFence vkFence = fence ? (VkFence)fence->fenceHandle : VK_NULL_HANDLE;
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // get count of all semaphores and command buffers
    uint32_t waitSemaphoreCount = 0, signalSemaphoreCount = 0, commandBufferCount = 0;
    for (uint32_t i = 0; i < submitCount; i++)
    {
        waitSemaphoreCount += pSubmits[i].waitSemaphoreCount;
        signalSemaphoreCount += pSubmits[i].signalSemaphoreCount;
        commandBufferCount += pSubmits[i].commandBufferCount;
    }

    // arrays to store semaphores and command buffers for submit infos
    VkSemaphore* waitSemaphores = lvn_memArenaAlloc(graphicsctx->frameArena, waitSemaphoreCount * sizeof(VkSemaphore));
    VkSemaphore* signalSemaphores = lvn_memArenaAlloc(graphicsctx->frameArena, signalSemaphoreCount * sizeof(VkSemaphore));
    VkCommandBuffer* commandBuffers = lvn_memArenaAlloc(graphicsctx->frameArena, commandBufferCount * sizeof(VkCommandBuffer));
    uint32_t waitSemaphoreOffset = 0, signalSemaphoreOffset = 0, commandBufferOffset = 0;

    VkSubmitInfo* submitInfos = lvn_memArenaAlloc(graphicsctx->frameArena, submitCount * sizeof(VkSubmitInfo));
    memset(submitInfos, 0, submitCount * sizeof(VkSubmitInfo));

    for (uint32_t i = 0; i < submitCount; i++)
    {
        // get vulkan semaphore/command buffer handles
        for (uint32_t j = 0; j < pSubmits[i].waitSemaphoreCount; j++)
            waitSemaphores[waitSemaphoreOffset + j] = (VkSemaphore) pSubmits[i].pWaitSemaphores[j]->semaphoreHandle;

        for (uint32_t j = 0; j < pSubmits[i].signalSemaphoreCount; j++)
            signalSemaphores[signalSemaphoreOffset + j] = (VkSemaphore) pSubmits[i].pSignalSemaphores[j]->semaphoreHandle;

        for (uint32_t j = 0; j < pSubmits[i].commandBufferCount; j++)
            commandBuffers[commandBufferOffset + j] = (VkCommandBuffer) pSubmits[i].pCommandBuffers[j]->commandbuffer;

        submitInfos[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfos[i].waitSemaphoreCount = pSubmits[i].waitSemaphoreCount;
        submitInfos[i].pWaitSemaphores = &waitSemaphores[waitSemaphoreOffset];
        submitInfos[i].pWaitDstStageMask = &waitStage;
        submitInfos[i].signalSemaphoreCount = pSubmits[i].signalSemaphoreCount;
        submitInfos[i].pSignalSemaphores = &signalSemaphores[signalSemaphoreOffset];
        submitInfos[i].commandBufferCount = pSubmits[i].commandBufferCount;
        submitInfos[i].pCommandBuffers = &commandBuffers[commandBufferOffset];

        // offset arrays for each submit info
        waitSemaphoreOffset += pSubmits[i].waitSemaphoreCount;
        signalSemaphoreOffset += pSubmits[i].signalSemaphoreCount;
        commandBufferOffset += pSubmits[i].commandBufferCount;
    }

    // queue submit
    if (vkBackends->queueSubmit(vkBackends->graphicsQueue, submitCount, submitInfos, vkFence) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to submit command buffers to queue");
        return Lvn_Result_Failure;
    }

    lvn_memArenaReset(graphicsctx->frameArena);

    return Lvn_Result_Success;
}

LvnResult lvnImplVkRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo)
{
    LVN_ASSERT(graphicsctx && presentInfo, "graphicsctx and presentInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkSemaphore* waitSemaphores = lvn_memArenaAlloc(graphicsctx->frameArena,
                                                    presentInfo->waitSemaphoreCount * sizeof(VkSemaphore));
    for (uint32_t i = 0; i < presentInfo->waitSemaphoreCount; i++)
        waitSemaphores[i] = (VkSemaphore) presentInfo->pWaitSemaphores[i]->semaphoreHandle;

    VkSwapchainKHR* swapchains = lvn_memArenaAlloc(graphicsctx->frameArena,
                                                   presentInfo->swapchainCount * sizeof(VkSwapchainKHR));
    for (uint32_t i = 0; i < presentInfo->swapchainCount; i++)
        swapchains[i] = ((LvnVkSwapchainData*)presentInfo->pSwapchains[i]->swapchainData)->swapchain;

    VkPresentInfoKHR vkPresentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = presentInfo->waitSemaphoreCount,
        .pWaitSemaphores = waitSemaphores,
        .swapchainCount = presentInfo->swapchainCount,
        .pSwapchains = swapchains,
        .pImageIndices = presentInfo->pImageIndices,
        .pResults = NULL,
    };

    VkResult result = vkBackends->queuePresentKHR(vkBackends->presentQueue, &vkPresentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        return Lvn_Result_OutOfDate;
    else if (result != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to present swapchain image");
        return Lvn_Result_Failure;
    }

    lvn_memArenaReset(graphicsctx->frameArena);

    return Lvn_Result_Success;
}
