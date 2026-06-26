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

static PFN_vkVoidFunction          lvn_getVulkanCreateSurfaceProcAddr(const LvnVulkanBackends* vkBackends);
static LvnResult                   lvn_createPlatformSurface(const LvnVulkanBackends* vkBackends, VkSurfaceKHR* surface, const LvnPlatformData* platformData);
static LvnVkQueueFamilyIndices     lvn_findQueueFamilies(const LvnVulkanBackends* vkBackends, VkPhysicalDevice device, VkSurfaceKHR surface);
static bool                        lvn_checkDeviceExtensionSupport(const LvnVulkanBackends* vkBackends, VkPhysicalDevice device, const char** requiredExtensions, uint32_t requiredExtensionCount);
static VkPhysicalDevice            lvn_getBestPhysicalDevice(const LvnVulkanBackends* vkBackends, VkSurfaceKHR surface);
static LvnResult                   lvn_createSwapChainData(const LvnVulkanBackends* vkBackends, LvnVkSwapchainData* swapchainData, const LvnVkSwapChainCreateInfo* createInfo);
static VkShaderStageFlagBits       lvn_getVkShaderStageEnum(LvnShaderStage stage);
static VkFormat                    lvn_getVkVertexAttributeFormatEnum(LvnAttributeFormat format);
static VkPrimitiveTopology         lvn_getVkTopologyTypeEnum(LvnTopologyType topologyType);
static VkPolygonMode               lvn_getVkPolygonModeEnum(LvnPolygonMode polygonMode);
static VkCullModeFlags             lvn_getVkCullModeFlagEnum(LvnCullFaceMode cullFaceMode);
static VkFrontFace                 lvn_getVkCullFrontFaceEnum(LvnCullFrontFace cullFrontFace);
static VkSampleCountFlagBits       lvn_getVkSampleCountFlagEnum(LvnSampleCountFlagBits samples);
static VkColorComponentFlags       lvn_getVkColorComponentsFlagEnum(LvnColorComponentFlags colorMask);
static VkBlendFactor               lvn_getVkBlendFactorEnum(LvnColorBlendFactor blendFactor);
static VkBlendOp                   lvn_getVkBlendOperationEnum(LvnColorBlendOperation blendOp);
static VkCompareOp                 lvn_getVkCompareOpEnum(LvnCompareOperation compare);
static VkStencilOp                 lvn_getVkStencilOpEnum(LvnStencilOperation stencilOp);
static VkLogicOp                   lvn_getVkLogicOpEnum(LvnLogicOperation logicOp);
static VkAttachmentLoadOp          lvn_getVkAttackmentLoadOpEnum(LvnAttachmentLoadOp loadOp);
static VkAttachmentStoreOp         lvn_getVkAttackmentStoreOpEnum(LvnAttachmentStoreOp storeOp);
static VkFormat                    lvn_getVkFormatEnum(LvnFormat format);
static VkPresentModeKHR            lvn_getVkPresentModeEnum(LvnPresentMode presentMode);
static VkFilter                    lvn_getVkTextureFilterEnum(LvnTextureFilter filter);
static VkSamplerAddressMode        lvn_getVkTextureModeEnum(LvnTextureMode mode);
static VkBufferUsageFlags          lvn_getVkBufferUsageFlagsEnum(LvnBufferTypeFlags type);
static LvnFormat                   lvn_getLvnFormatEnum(VkFormat format);
static LvnPresentMode              lvn_getLvnPresentModeEnum(VkPresentModeKHR presentMode);
static void                        lvn_transitionImageLayout(const LvnVulkanBackends* vkBackends, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount);
static LvnResult                   lvn_createBuffer(const LvnVulkanBackends* vkBackends, VkBuffer* buffer, VmaAllocation* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage);
static void                        lvn_copyBuffer(const LvnVulkanBackends* vkBackends, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset);
static LvnResult                   lvn_createImage(const LvnVulkanBackends* vkBackends, VkImage* image, VmaAllocation* imageMemory, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlagBits samples, VmaMemoryUsage memUsage);
static void                        lvn_copyBufferToImage(const LvnVulkanBackends* vkBackends, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

static VKAPI_ATTR VkBool32 VKAPI_CALL lvn_vulkanDebugCallback(
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

static PFN_vkVoidFunction lvn_getVulkanCreateSurfaceProcAddr(const LvnVulkanBackends* vkBackends)
{
    LVN_ASSERT(vkBackends, "vkBackends cannot be null");

    LvnWindowPlatformSupport windowSupport = {0};
    lvn_getWindowPlatform(&windowSupport);
#if defined(LVN_INCLUDE_WAYLAND)
    if (windowSupport.waylandSupport)
        return vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkCreateWaylandSurfaceKHR");
#endif
#if defined(LVN_INCLUDE_X11)
    if (windowSupport.x11Support)
        return vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkCreateXlibSurfaceKHR");
#endif

    return VK_NULL_HANDLE;
}

static LvnResult lvn_createPlatformSurface(const LvnVulkanBackends* vkBackends, VkSurfaceKHR* surface, const LvnPlatformData* platformData)
{
    VkResult result = VK_ERROR_UNKNOWN;

#if defined(LVN_INCLUDE_X11) || defined(LVN_INCLUDE_WAYLAND)
    LvnWindowPlatformSupport windowSupport = {0};
    lvn_getWindowPlatform(&windowSupport);

    if (windowSupport.waylandSupport)
    {
        VkWaylandSurfaceCreateInfoKHR sci = {
            .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
            .display = (struct wl_display*) platformData->ndh,
            .surface = (struct wl_surface*) platformData->nwh,
        };
        PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR_PFN =
            (PFN_vkCreateWaylandSurfaceKHR) vkBackends->vkCreateSurfaceProc;
        result = vkCreateWaylandSurfaceKHR_PFN(vkBackends->instance, &sci, NULL, surface);
    }
    else if (windowSupport.x11Support)
    {
        VkXlibSurfaceCreateInfoKHR sci = {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .dpy = (Display*) platformData->ndh,
            .window = *(Window*) platformData->nwh,
        };
        PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR =
            (PFN_vkCreateXlibSurfaceKHR) vkBackends->vkCreateSurfaceProc;
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

    vkBackends->vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    queueFamilies = lvn_calloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkBackends->vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

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
            vkBackends->vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

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

    vkBackends->vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL);
    extensions = lvn_calloc(extensionCount * sizeof(VkExtensionProperties));
    vkBackends->vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, extensions);

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

    vkBackends->vkEnumeratePhysicalDevices(vkBackends->instance, &physicalDeviceCount, NULL);
    physicalDevices = lvn_calloc(physicalDeviceCount * sizeof(VkPhysicalDevice));
    vkBackends->vkEnumeratePhysicalDevices(vkBackends->instance, &physicalDeviceCount, physicalDevices);

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
        vkBackends->vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

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

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkImage* swapchainImages = NULL;
    VkImageView* swapchainImageViews = NULL;
    uint32_t swapchainImageCount = 0;

    // check for swapchain capabilitie support
    VkSurfaceCapabilitiesKHR capabilities;
    vkBackends->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(createInfo->physicalDevice, createInfo->surface, &capabilities);

    // swapchain present modes
    uint32_t presentModeCount;
    vkBackends->vkGetPhysicalDeviceSurfacePresentModesKHR(createInfo->physicalDevice, createInfo->surface, &presentModeCount, NULL);

    if (!presentModeCount)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to create swapchain, no supported present mode found");
        goto fail_cleanup;
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
    swapchainCreateInfo.presentMode = createInfo->presentMode;
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

    if (vkBackends->vkCreateSwapchainKHR(vkBackends->device, &swapchainCreateInfo, NULL, &swapchain) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to create swapchain");
        goto fail_cleanup;
    }

    // get swapchain images
    vkBackends->vkGetSwapchainImagesKHR(vkBackends->device, swapchain, &swapchainImageCount, NULL);
    swapchainImages = lvn_calloc(swapchainImageCount * sizeof(VkImage));
    vkBackends->vkGetSwapchainImagesKHR(vkBackends->device, swapchain, &swapchainImageCount, swapchainImages);

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

        if (vkBackends->vkCreateImageView(vkBackends->device, &imageViewCreateInfo, NULL, &swapchainImageViews[i]) != VK_SUCCESS)
        {
            LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to create swapchain image views");
            goto fail_cleanup;
        }
    }

    swapchainData->surface = createInfo->surface;
    swapchainData->swapchain = swapchain;
    swapchainData->swapchainFormat = createInfo->surfaceFormat;
    swapchainData->presentMode = createInfo->presentMode;
    swapchainData->swapchainExtent = extent;
    swapchainData->swapchainImageCount = swapchainImageCount;
    swapchainData->swapchainImages = swapchainImages;
    swapchainData->swapchainImageViews = swapchainImageViews;

    return Lvn_Result_Success;

fail_cleanup:
    for (uint32_t i = 0; i < swapchainImageCount; i++)
        vkBackends->vkDestroyImageView(vkBackends->device, swapchainImageViews[i], NULL);
    vkBackends->vkDestroySwapchainKHR(vkBackends->device, swapchain, NULL);
    lvn_free(swapchainImageViews);
    lvn_free(swapchainImages);
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

static VkPolygonMode lvn_getVkPolygonModeEnum(LvnPolygonMode polygonMode)
{
    switch (polygonMode)
    {
        case Lvn_PolygonMode_Fill: { return VK_POLYGON_MODE_FILL; }
        case Lvn_PolygonMode_Line: { return VK_POLYGON_MODE_LINE; }
        case Lvn_PolygonMode_Point: { return VK_POLYGON_MODE_POINT; }
    }

    LVN_ASSERT(false, "invalid polygon mode enum");
    return VK_POLYGON_MODE_FILL;
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

static VkLogicOp lvn_getVkLogicOpEnum(LvnLogicOperation logicOp)
{
    switch (logicOp)
    {
        case Lvn_LogicOp_Clear: { return VK_LOGIC_OP_CLEAR; }
        case Lvn_LogicOp_And: { return VK_LOGIC_OP_AND; }
        case Lvn_LogicOp_AndReverse: { return VK_LOGIC_OP_AND_REVERSE; }
        case Lvn_LogicOp_Copy: { return VK_LOGIC_OP_COPY; }
        case Lvn_LogicOp_AndInverted: { return VK_LOGIC_OP_AND_INVERTED; }
        case Lvn_LogicOp_NoOp: { return VK_LOGIC_OP_NO_OP; }
        case Lvn_LogicOp_Xor: { return VK_LOGIC_OP_XOR; }
        case Lvn_LogicOp_Or: { return VK_LOGIC_OP_OR; }
        case Lvn_LogicOp_Nor: { return VK_LOGIC_OP_NOR; }
        case Lvn_LogicOp_Equivalent: { return VK_LOGIC_OP_EQUIVALENT; }
        case Lvn_LogicOp_Invert: { return VK_LOGIC_OP_INVERT; }
        case Lvn_LogicOp_OrReverse: { return VK_LOGIC_OP_OR_REVERSE; }
        case Lvn_LogicOp_CopyInverted: { return VK_LOGIC_OP_COPY_INVERTED; }
        case Lvn_LogicOp_OrInverted: { return VK_LOGIC_OP_OR_INVERTED; }
        case Lvn_LogicOp_Nand: { return VK_LOGIC_OP_NAND; }
        case Lvn_LogicOp_Set: { return VK_LOGIC_OP_SET; }
    }

    LVN_ASSERT(false, "invalid logic operation enum");
    return VK_LOGIC_OP_CLEAR;
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

static VkFormat lvn_getVkFormatEnum(LvnFormat format)
{
    switch (format)
    {
        case Lvn_Format_Undefined: { return VK_FORMAT_UNDEFINED; }
        case Lvn_Format_R8_UNORM: { return VK_FORMAT_R8_UNORM; }
        case Lvn_Format_R8_SNORM: { return VK_FORMAT_R8_SNORM; }
        case Lvn_Format_R8_UINT: { return VK_FORMAT_R8_UINT; }
        case Lvn_Format_R8_SINT: { return VK_FORMAT_R8_SINT; }
        case Lvn_Format_R16_UNORM: { return VK_FORMAT_R16_UNORM; }
        case Lvn_Format_R16_SNORM: { return VK_FORMAT_R16_SNORM; }
        case Lvn_Format_R16_UINT: { return VK_FORMAT_R16_UINT; }
        case Lvn_Format_R16_SINT: { return VK_FORMAT_R16_SINT; }
        case Lvn_Format_R16_FLOAT: { return VK_FORMAT_R16_SFLOAT; }
        case Lvn_Format_R32_UINT: { return VK_FORMAT_R32_UINT; }
        case Lvn_Format_R32_SINT: { return VK_FORMAT_R32_SINT; }
        case Lvn_Format_R32_FLOAT: { return VK_FORMAT_R32_SFLOAT; }
        case Lvn_Format_R8G8_UNORM: { return VK_FORMAT_R8G8_UNORM; }
        case Lvn_Format_R8G8_SNORM: { return VK_FORMAT_R8G8_SNORM; }
        case Lvn_Format_R8G8_UINT: { return VK_FORMAT_R8G8_UINT; }
        case Lvn_Format_R8G8_SINT: { return VK_FORMAT_R8G8_SINT; }
        case Lvn_Format_R16G16_FLOAT: { return VK_FORMAT_R16G16_SFLOAT; }
        case Lvn_Format_R32G32_FLOAT: { return VK_FORMAT_R32G32_SFLOAT; }
        case Lvn_Format_R32G32_UINT: { return VK_FORMAT_R32G32_UINT; }
        case Lvn_Format_R32G32_SINT: { return VK_FORMAT_R32G32_SINT; }
        case Lvn_Format_R32G32B32_FLOAT: { return VK_FORMAT_R32G32B32_SFLOAT; }
        case Lvn_Format_R32G32B32_UINT: { return VK_FORMAT_R32G32B32_UINT; }
        case Lvn_Format_R32G32B32_SINT: { return VK_FORMAT_R32G32B32_SINT; }
        case Lvn_Format_R8G8B8A8_UNORM: { return VK_FORMAT_R8G8B8A8_UNORM; }
        case Lvn_Format_R8G8B8A8_SNORM: { return VK_FORMAT_R8G8B8A8_SNORM; }
        case Lvn_Format_R8G8B8A8_UINT: { return VK_FORMAT_R8G8B8A8_UINT; }
        case Lvn_Format_R8G8B8A8_SINT: { return VK_FORMAT_R8G8B8A8_SINT; }
        case Lvn_Format_R8G8B8A8_SRGB: { return VK_FORMAT_R8G8B8A8_SRGB; }
        case Lvn_Format_R16G16B16A16_FLOAT: { return VK_FORMAT_R16G16B16A16_SFLOAT; }
        case Lvn_Format_R32G32B32A32_FLOAT: { return VK_FORMAT_R32G32B32A32_SFLOAT; }
        case Lvn_Format_R32G32B32A32_UINT: { return VK_FORMAT_R32G32B32A32_UINT; }
        case Lvn_Format_R32G32B32A32_SINT: { return VK_FORMAT_R32G32B32A32_SINT; }
        case Lvn_Format_B8G8R8A8_UNORM: { return VK_FORMAT_B8G8R8A8_UNORM; }
        case Lvn_Format_B8G8R8A8_SRGB: { return VK_FORMAT_B8G8R8A8_SRGB; }
        case Lvn_Format_A2B10G10R10_UNORM: { return VK_FORMAT_A2B10G10R10_UNORM_PACK32; }
        case Lvn_Format_A2B10G10R10_UINT: { return VK_FORMAT_A2B10G10R10_UINT_PACK32; }
        case Lvn_Format_D16_UNORM: { return VK_FORMAT_D16_UNORM; }
        case Lvn_Format_D24_UNORM_S8_UINT: { return VK_FORMAT_D24_UNORM_S8_UINT; }
        case Lvn_Format_D32_FLOAT: { return VK_FORMAT_D32_SFLOAT; }
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

static VkBufferUsageFlags lvn_getVkBufferUsageFlagsEnum(LvnBufferTypeFlags type)
{
    VkBufferUsageFlags usageFlags = 0;

    if (type & Lvn_BufferTypeFlag_Vertex)
        usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (type & Lvn_BufferTypeFlag_Index)
        usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (type & Lvn_BufferTypeFlag_Uniform)
        usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (type & Lvn_BufferTypeFlag_Storage)
        usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    LVN_ASSERT(usageFlags, "invalid buffer usage flags enum");

    return usageFlags;
}

static LvnFormat lvn_getLvnFormatEnum(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_UNDEFINED: { return Lvn_Format_Undefined; }
        case VK_FORMAT_R8_UNORM: { return Lvn_Format_R8_UNORM; }
        case VK_FORMAT_R16_SFLOAT: { return Lvn_Format_R16_FLOAT; }
        case VK_FORMAT_R32_SFLOAT: { return Lvn_Format_R32_FLOAT; }
        case VK_FORMAT_R8G8_UNORM: { return Lvn_Format_R8G8_UNORM; }
        case VK_FORMAT_R16G16_SFLOAT: { return Lvn_Format_R16G16_FLOAT; }
        case VK_FORMAT_R32G32_SFLOAT: { return Lvn_Format_R32G32_FLOAT; }
        case VK_FORMAT_R8G8B8A8_UNORM: { return Lvn_Format_R8G8B8A8_UNORM; }
        case VK_FORMAT_R8G8B8A8_SRGB: { return Lvn_Format_R8G8B8A8_SRGB; }
        case VK_FORMAT_R16G16B16A16_SFLOAT: { return Lvn_Format_R16G16B16A16_FLOAT; }
        case VK_FORMAT_R32G32B32A32_SFLOAT: { return Lvn_Format_R32G32B32A32_FLOAT; }
        case VK_FORMAT_B8G8R8A8_UNORM: { return Lvn_Format_B8G8R8A8_UNORM; }
        case VK_FORMAT_B8G8R8A8_SRGB: { return Lvn_Format_B8G8R8A8_SRGB; }
        case VK_FORMAT_D24_UNORM_S8_UINT: { return Lvn_Format_D24_UNORM_S8_UINT; }
        case VK_FORMAT_D32_SFLOAT: { return Lvn_Format_D32_FLOAT; }
        default: { break; }
    }

    return Lvn_Format_Undefined;
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
    vkBackends->vkAllocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBackends->vkBeginCommandBuffer(commandBuffer, &beginInfo);

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
        vkBackends->vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
        return;
    }

    vkBackends->vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
    vkBackends->vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vkBackends->vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkBackends->vkQueueWaitIdle(vkBackends->graphicsQueue);

    vkBackends->vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
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
    vkBackends->vkAllocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBackends->vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {
        .size = size,
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
    };

    vkBackends->vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkBackends->vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vkBackends->vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkBackends->vkQueueWaitIdle(vkBackends->graphicsQueue);

    vkBackends->vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
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
    vkBackends->vkAllocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBackends->vkBeginCommandBuffer(commandBuffer, &beginInfo);

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

    vkBackends->vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    vkBackends->vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vkBackends->vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkBackends->vkQueueWaitIdle(vkBackends->graphicsQueue);

    vkBackends->vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
}

LvnResult lvnImplVkInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && createInfo, "graphicsctx and createInfo cannot be nullptr");

    LvnResult errResult = Lvn_Result_Failure;
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
                      "[vulkan] failed to load vulkan shared library: %s",
                      s_LvnVkLibName);
        goto fail_cleanup;
    }

    // vulkan get instace proc address
    vkBackends->vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
        lvn_platformGetModuleSymbol(vkBackends->handle, "vkGetInstanceProcAddr");

    if (!vkBackends->vkGetInstanceProcAddr)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to retrieve vkGetInstanceProcAddr symbol");
        goto fail_cleanup;
    }

    // vulkan global level function symbols
    vkBackends->vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)
        vkBackends->vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
    vkBackends->vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)
        vkBackends->vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
    vkBackends->vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)
        vkBackends->vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");
    vkBackends->vkCreateInstance = (PFN_vkCreateInstance)
        vkBackends->vkGetInstanceProcAddr(NULL, "vkCreateInstance");


    if (!vkBackends->vkEnumerateInstanceVersion ||
        !vkBackends->vkEnumerateInstanceExtensionProperties ||
        !vkBackends->vkEnumerateInstanceLayerProperties ||
        !vkBackends->vkCreateInstance)
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
        VkResult result = vkBackends->vkEnumerateInstanceExtensionProperties(NULL, &extensionPropsCount, NULL);
        if (result != VK_SUCCESS)
        {
            // NOTE: this happens on systems with a loader but without any vulkan ICD
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to query vulkan instance extensions");
            goto fail_cleanup;
        }

        extensionProps = (VkExtensionProperties*) lvn_calloc(extensionPropsCount * sizeof(VkExtensionProperties));
        result = vkBackends->vkEnumerateInstanceExtensionProperties(NULL, &extensionPropsCount, extensionProps);
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
        vkBackends->vkEnumerateInstanceLayerProperties(&availableLayerCount, NULL);
        availableLayers = (VkLayerProperties*) lvn_calloc(availableLayerCount * sizeof(VkLayerProperties));
        vkBackends->vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers);

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
    debugCreateInfo.pfnUserCallback = lvn_vulkanDebugCallback;
    debugCreateInfo.pUserData = graphicsctx;

    // get vulkan version
    uint32_t vulkanVersion;
    vkBackends->vkEnumerateInstanceVersion(&vulkanVersion);
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

    if (vkBackends->vkCreateInstance(&vkCreateInfo, NULL, &vkBackends->instance) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create instance");
        goto fail_cleanup;
    }

    // get instance level function symbols
    vkBackends->vkDestroyInstance = (PFN_vkDestroyInstance)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkDestroyInstance");
    vkBackends->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkEnumeratePhysicalDevices");
    vkBackends->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    vkBackends->vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkEnumerateDeviceExtensionProperties");
    vkBackends->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceProperties");
    vkBackends->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceMemoryProperties");
    vkBackends->vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceFormatProperties");
    vkBackends->vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceFeatures");
    vkBackends->vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetDeviceProcAddr");
    vkBackends->vkCreateDevice = (PFN_vkCreateDevice)
        vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkCreateDevice");

    if (!vkBackends->vkDestroyInstance ||
        !vkBackends->vkEnumeratePhysicalDevices ||
        !vkBackends->vkGetPhysicalDeviceQueueFamilyProperties ||
        !vkBackends->vkEnumerateDeviceExtensionProperties ||
        !vkBackends->vkGetPhysicalDeviceProperties ||
        !vkBackends->vkGetPhysicalDeviceMemoryProperties ||
        !vkBackends->vkGetPhysicalDeviceFeatures ||
        !vkBackends->vkGetDeviceProcAddr ||
        !vkBackends->vkCreateDevice)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to load vulkan instance level function symbols");
        goto fail_cleanup;
    }

    if (createInfo->presentationModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        vkBackends->vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
            vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        vkBackends->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
            vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        vkBackends->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)
            vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        vkBackends->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)
            vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
        vkBackends->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)
            vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkDestroySurfaceKHR");


        // get create surface PFN based on window platform
        vkBackends->vkCreateSurfaceProc = lvn_getVulkanCreateSurfaceProcAddr(vkBackends);

        if (!vkBackends->vkGetPhysicalDeviceSurfaceSupportKHR ||
            !vkBackends->vkGetPhysicalDeviceSurfaceCapabilitiesKHR ||
            !vkBackends->vkGetPhysicalDeviceSurfaceFormatsKHR ||
            !vkBackends->vkGetPhysicalDeviceSurfacePresentModesKHR ||
            !vkBackends->vkDestroySurfaceKHR ||
            !vkBackends->vkCreateSurfaceProc)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to load vulkan instance level surface function symbol");
            goto fail_cleanup;
        }
    }

    // create debug messegenger if debug logging enabled
    if (vkBackends->enableValidationLayers)
    {
        vkBackends->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
            vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkCreateDebugUtilsMessengerEXT");
        vkBackends->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkBackends->vkGetInstanceProcAddr(vkBackends->instance, "vkDestroyDebugUtilsMessengerEXT");

        if (!vkBackends->vkCreateDebugUtilsMessengerEXT ||
            !vkBackends->vkDestroyDebugUtilsMessengerEXT)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to load vulkan debug message function symbols");
            goto fail_cleanup;
        }

        if (vkBackends->vkCreateDebugUtilsMessengerEXT(vkBackends->instance, &debugCreateInfo, NULL, &vkBackends->debugMessenger) != VK_SUCCESS)
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
    vkBackends->vkGetPhysicalDeviceProperties(vkBackends->physicalDevice, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkBackends->vkGetPhysicalDeviceFeatures(vkBackends->physicalDevice, &deviceFeatures);

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

    const char* requiredExtensions = NULL;
    uint32_t requiredExtensionCount = 0;

    // enable device features
    VkPhysicalDeviceFeatures enabledDeviceFeatures = {0};

    // sampler anisotropy
    if (deviceFeatures.samplerAnisotropy)
        enabledDeviceFeatures.samplerAnisotropy = VK_TRUE;

    deviceCreateInfo.pEnabledFeatures = &enabledDeviceFeatures;

    if (graphicsctx->presentModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        requiredExtensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        requiredExtensionCount = 1;
        deviceCreateInfo.ppEnabledExtensionNames = &requiredExtensions;
        deviceCreateInfo.enabledExtensionCount = requiredExtensionCount;
    }

    if (vkBackends->vkCreateDevice(vkBackends->physicalDevice, &deviceCreateInfo, NULL, &vkBackends->device) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create logical device");
        goto fail_cleanup;
    }

    // get device level function symbols
    vkBackends->vkDestroyDevice = (PFN_vkDestroyDevice)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyDevice");
    vkBackends->vkGetDeviceQueue = (PFN_vkGetDeviceQueue)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkGetDeviceQueue");
    vkBackends->vkCreateImage = (PFN_vkCreateImage)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateImage");
    vkBackends->vkDestroyImage = (PFN_vkDestroyImage)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyImage");
    vkBackends->vkCreateImageView = (PFN_vkCreateImageView)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateImageView");
    vkBackends->vkDestroyImageView = (PFN_vkDestroyImageView)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyImageView");
    vkBackends->vkCreateSampler = (PFN_vkCreateSampler)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateSampler");
    vkBackends->vkDestroySampler = (PFN_vkDestroySampler)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroySampler");
    vkBackends->vkCreateShaderModule = (PFN_vkCreateShaderModule)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateShaderModule");
    vkBackends->vkDestroyShaderModule = (PFN_vkDestroyShaderModule)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyShaderModule");
    vkBackends->vkCreateRenderPass = (PFN_vkCreateRenderPass)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateRenderPass");
    vkBackends->vkDestroyRenderPass = (PFN_vkDestroyRenderPass)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyRenderPass");
    vkBackends->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreatePipelineLayout");
    vkBackends->vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyPipelineLayout");
    vkBackends->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateGraphicsPipelines");
    vkBackends->vkDestroyPipeline = (PFN_vkDestroyPipeline)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyPipeline");
    vkBackends->vkCreateFramebuffer = (PFN_vkCreateFramebuffer)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateFramebuffer");
    vkBackends->vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyFramebuffer");
    vkBackends->vkCreateBuffer = (PFN_vkCreateBuffer)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateBuffer");
    vkBackends->vkDestroyBuffer = (PFN_vkDestroyBuffer)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyBuffer");
    vkBackends->vkCreateFence = (PFN_vkCreateFence)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateFence");
    vkBackends->vkDestroyFence = (PFN_vkDestroyFence)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyFence");
    vkBackends->vkCreateSemaphore = (PFN_vkCreateSemaphore)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateSemaphore");
    vkBackends->vkDestroySemaphore = (PFN_vkDestroySemaphore)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroySemaphore");
    vkBackends->vkCreateCommandPool = (PFN_vkCreateCommandPool)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateCommandPool");
    vkBackends->vkDestroyCommandPool = (PFN_vkDestroyCommandPool)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroyCommandPool");
    vkBackends->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkAllocateCommandBuffers");
    vkBackends->vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkFreeCommandBuffers");
    vkBackends->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkBeginCommandBuffer");
    vkBackends->vkEndCommandBuffer = (PFN_vkEndCommandBuffer)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkEndCommandBuffer");
    vkBackends->vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdBeginRenderPass");
    vkBackends->vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdEndRenderPass");
    vkBackends->vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdPipelineBarrier");
    vkBackends->vkCmdBindPipeline = (PFN_vkCmdBindPipeline)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdBindPipeline");
    vkBackends->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdBindVertexBuffers");
    vkBackends->vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdBindIndexBuffer");
    vkBackends->vkCmdSetViewport = (PFN_vkCmdSetViewport)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdSetViewport");
    vkBackends->vkCmdSetScissor = (PFN_vkCmdSetScissor)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdSetScissor");
    vkBackends->vkCmdDraw = (PFN_vkCmdDraw)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdDraw");
    vkBackends->vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdDrawIndexed");
    vkBackends->vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdCopyBuffer");
    vkBackends->vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCmdCopyBufferToImage");
    vkBackends->vkQueueSubmit = (PFN_vkQueueSubmit)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkQueueSubmit");
    vkBackends->vkWaitForFences = (PFN_vkWaitForFences)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkWaitForFences");
    vkBackends->vkResetFences = (PFN_vkResetFences)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkResetFences");
    vkBackends->vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDeviceWaitIdle");
    vkBackends->vkQueueWaitIdle = (PFN_vkQueueWaitIdle)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkQueueWaitIdle");
    vkBackends->vkAllocateMemory = (PFN_vkAllocateMemory)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkAllocateMemory");
    vkBackends->vkFreeMemory = (PFN_vkFreeMemory)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkFreeMemory");
    vkBackends->vkMapMemory = (PFN_vkMapMemory)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkMapMemory");
    vkBackends->vkUnmapMemory = (PFN_vkUnmapMemory)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkUnmapMemory");
    vkBackends->vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkFlushMappedMemoryRanges");
    vkBackends->vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkInvalidateMappedMemoryRanges");
    vkBackends->vkBindBufferMemory = (PFN_vkBindBufferMemory)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkBindBufferMemory");
    vkBackends->vkBindImageMemory = (PFN_vkBindImageMemory)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkBindImageMemory");
    vkBackends->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkGetBufferMemoryRequirements");
    vkBackends->vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)
        vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkGetImageMemoryRequirements");

    if (!vkBackends->vkDestroyDevice ||
        !vkBackends->vkGetDeviceQueue ||
        !vkBackends->vkCreateImage ||
        !vkBackends->vkDestroyImage ||
        !vkBackends->vkCreateImageView ||
        !vkBackends->vkDestroyImageView ||
        !vkBackends->vkCreateSampler ||
        !vkBackends->vkDestroySampler ||
        !vkBackends->vkCreateShaderModule ||
        !vkBackends->vkDestroyShaderModule ||
        !vkBackends->vkCreateRenderPass ||
        !vkBackends->vkDestroyRenderPass ||
        !vkBackends->vkCreatePipelineLayout ||
        !vkBackends->vkDestroyPipelineLayout ||
        !vkBackends->vkCreateGraphicsPipelines ||
        !vkBackends->vkDestroyPipeline ||
        !vkBackends->vkCreateFramebuffer ||
        !vkBackends->vkDestroyFramebuffer ||
        !vkBackends->vkCreateBuffer ||
        !vkBackends->vkDestroyBuffer ||
        !vkBackends->vkCreateFence ||
        !vkBackends->vkDestroyFence ||
        !vkBackends->vkCreateSemaphore ||
        !vkBackends->vkDestroySemaphore ||
        !vkBackends->vkCreateCommandPool ||
        !vkBackends->vkDestroyCommandPool ||
        !vkBackends->vkAllocateCommandBuffers ||
        !vkBackends->vkBeginCommandBuffer ||
        !vkBackends->vkEndCommandBuffer ||
        !vkBackends->vkCmdBeginRenderPass ||
        !vkBackends->vkCmdEndRenderPass ||
        !vkBackends->vkCmdPipelineBarrier ||
        !vkBackends->vkCmdBindPipeline ||
        !vkBackends->vkCmdBindVertexBuffers ||
        !vkBackends->vkCmdBindIndexBuffer ||
        !vkBackends->vkCmdSetViewport ||
        !vkBackends->vkCmdSetScissor ||
        !vkBackends->vkCmdDraw ||
        !vkBackends->vkCmdDrawIndexed ||
        !vkBackends->vkCmdCopyBuffer ||
        !vkBackends->vkCmdCopyBufferToImage ||
        !vkBackends->vkQueueSubmit ||
        !vkBackends->vkWaitForFences ||
        !vkBackends->vkResetFences ||
        !vkBackends->vkDeviceWaitIdle ||
        !vkBackends->vkAllocateMemory ||
        !vkBackends->vkFreeMemory ||
        !vkBackends->vkMapMemory ||
        !vkBackends->vkUnmapMemory ||
        !vkBackends->vkFlushMappedMemoryRanges ||
        !vkBackends->vkInvalidateMappedMemoryRanges ||
        !vkBackends->vkBindBufferMemory ||
        !vkBackends->vkBindImageMemory ||
        !vkBackends->vkGetBufferMemoryRequirements ||
        !vkBackends->vkGetImageMemoryRequirements)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to load vulkan device level function symbols");
        goto fail_cleanup;
    }

    if (graphicsctx->presentModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        vkBackends->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)
            vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkCreateSwapchainKHR");
        vkBackends->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)
            vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkDestroySwapchainKHR");
        vkBackends->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)
            vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkGetSwapchainImagesKHR");
        vkBackends->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)
            vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkAcquireNextImageKHR");
        vkBackends->vkQueuePresentKHR = (PFN_vkQueuePresentKHR)
            vkBackends->vkGetDeviceProcAddr(vkBackends->device, "vkQueuePresentKHR");

        if (!vkBackends->vkCreateSwapchainKHR ||
            !vkBackends->vkDestroySwapchainKHR ||
            !vkBackends->vkGetSwapchainImagesKHR ||
            !vkBackends->vkAcquireNextImageKHR ||
            !vkBackends->vkQueuePresentKHR)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to load vulkan device level surface function symbol");
            goto fail_cleanup;
        }

    }

    // get graphics and present queues from device
    vkBackends->vkGetDeviceQueue(vkBackends->device, vkBackends->queueFamilyIndices.graphicsIndex, 0, &vkBackends->graphicsQueue);

    if (graphicsctx->presentModeFlags & Lvn_PresentationModeFlag_Surface)
        vkBackends->vkGetDeviceQueue(vkBackends->device, vkBackends->queueFamilyIndices.presentIndex, 0, &vkBackends->presentQueue);


    // create command pool
    VkCommandPoolCreateInfo poolCreateInfo = {0};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolCreateInfo.queueFamilyIndex = vkBackends->queueFamilyIndices.graphicsIndex;
    if (vkBackends->vkCreateCommandPool(vkBackends->device, &poolCreateInfo, NULL, &vkBackends->commandPool) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create command pool");
        goto fail_cleanup;
    }

    // create vma allocator
    VmaVulkanFunctions vkFuncs = {
        .vkGetPhysicalDeviceProperties       = vkBackends->vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties = vkBackends->vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory                    = vkBackends->vkAllocateMemory,
        .vkFreeMemory                        = vkBackends->vkFreeMemory,
        .vkMapMemory                         = vkBackends->vkMapMemory,
        .vkUnmapMemory                       = vkBackends->vkUnmapMemory,
        .vkFlushMappedMemoryRanges           = vkBackends->vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges      = vkBackends->vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory                  = vkBackends->vkBindBufferMemory,
        .vkBindImageMemory                   = vkBackends->vkBindImageMemory,
        .vkGetBufferMemoryRequirements       = vkBackends->vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements        = vkBackends->vkGetImageMemoryRequirements,
        .vkCreateBuffer                      = vkBackends->vkCreateBuffer,
        .vkDestroyBuffer                     = vkBackends->vkDestroyBuffer,
        .vkCreateImage                       = vkBackends->vkCreateImage,
        .vkDestroyImage                      = vkBackends->vkDestroyImage,
        .vkCmdCopyBuffer                     = vkBackends->vkCmdCopyBuffer,
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

    LvnMemoryArenaCreateInfo arenaCreateInfo = {
        .size = 16e+3, // 16 KB
        .align = LVN_DEFAULT_ALIGN,
    };

    LvnResult result = lvn_memArenaCreate(&vkBackends->frameArena, &arenaCreateInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create frame arena");
        errResult = result;
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
    graphicsctx->implCreateBuffer = lvnImplVkCreateBuffer;
    graphicsctx->implDestroyBuffer = lvnImplVkDestroyBuffer;
    graphicsctx->implCreateSampler = lvnImplVkCreateSampler;
    graphicsctx->implDestroySampler = lvnImplVkDestroySampler;
    graphicsctx->implCreateTexture = lvnImplVkCreateTexture;
    graphicsctx->implDestroyTexture = lvnImplVkDestroyTexture;
    graphicsctx->implCreateCommandBuffer = lvnImplVkCreateCommandBuffer;
    graphicsctx->implDestroyCommandBuffer = lvnImplVkDestroyCommandBuffer;
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

    vkBackends->vkDestroySurfaceKHR(vkBackends->instance, surface, NULL);
    lvn_free(extensionProps);
    lvn_free(extensionNames);
    lvn_free(availableLayers);

    return Lvn_Result_Success;

fail_cleanup:
    vkBackends->vkDestroySurfaceKHR(vkBackends->instance, surface, NULL);
    lvn_free(extensionProps);
    lvn_free(extensionNames);
    lvn_free(availableLayers);
    lvnImplVkTerminate(graphicsctx);
    return errResult;
}

void lvnImplVkTerminate(LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    LvnVulkanBackends* vkBackends = (LvnVulkanBackends*) graphicsctx->implData;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    lvn_memArenaDestroy(&vkBackends->frameArena);

    if (vkBackends->vmaAllocator)
        vmaDestroyAllocator(vkBackends->vmaAllocator);
    if (vkBackends->commandPool)
        vkBackends->vkDestroyCommandPool(vkBackends->device, vkBackends->commandPool, NULL);
    if (vkBackends->device)
        vkBackends->vkDestroyDevice(vkBackends->device, NULL);
    if (vkBackends->debugMessenger)
        vkBackends->vkDestroyDebugUtilsMessengerEXT(vkBackends->instance, vkBackends->debugMessenger, NULL);
    if (vkBackends->instance)
        vkBackends->vkDestroyInstance(vkBackends->instance, NULL);

    if (vkBackends->handle)
        lvn_platformFreeModule(vkBackends->handle);

    lvn_free(vkBackends);
    graphicsctx->implData = NULL;
}

LvnResult lvnImplVkCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && surface && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    LvnPlatformData platformData = {
        .ndh = createInfo->nativeDisplayHandle,
        .nwh = createInfo->nativeWindowHandle,
    };

    VkSurfaceKHR vkSurface;
    if (lvn_createPlatformSurface(vkBackends, &vkSurface, &platformData) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create VkSurfaceKHR for surface %p", surface);
        return Lvn_Result_Failure;
    }

    surface->surfaceData = vkSurface;
    return Lvn_Result_Success;
}

void lvnImplVkDestroySurface(LvnSurface* surface)
{
    LVN_ASSERT(surface, "surface cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) surface->graphicsctx->implData;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    VkSurfaceKHR vkSurface = (VkSurfaceKHR) surface->surfaceData;
    vkBackends->vkDestroySurfaceKHR(vkBackends->instance, vkSurface, NULL);
    surface->surfaceData = NULL;
}

LvnResult lvnImplVkCreateSwapchain(const LvnGraphicsContext* graphicsctx, LvnSwapchain* swapchain, const LvnSwapchainCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && swapchain && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    VkSurfaceKHR vkSurface = (VkSurfaceKHR) createInfo->surface->surfaceData;

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
        goto fail_cleanup;
    }

    LvnResult result = lvn_createSwapChainData(vkBackends, swapchainData, &swapchainCreateInfo);

    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create swapchain data for swapchain %p", swapchain);
        errResult = result;
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
        swapchainImages[i].textureData = lvn_calloc(sizeof(LvnVkTextureData));

        if (!swapchainImages[i].textureData)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to allocate memory for texture data for image in swapchain %p", swapchain);
            errResult = Lvn_Result_OutOfMemory;
            goto fail_cleanup;
        }

        LvnVkTextureData* textureData = (LvnVkTextureData*) swapchainImages[i].textureData;

        textureData->image = swapchainData->swapchainImages[i];
        textureData->imageView = swapchainData->swapchainImageViews[i];
        textureData->width = swapchainData->swapchainExtent.width;
        textureData->height = swapchainData->swapchainExtent.height;

        swapchainImages[i].width = swapchainData->swapchainExtent.width;
        swapchainImages[i].height = swapchainData->swapchainExtent.height;
    }

    swapchain->swapchainData = swapchainData;
    swapchain->pSwapchainImages = swapchainImages;
    swapchain->swapchainImageCount = swapchainData->swapchainImageCount;
    swapchain->swapchainColorFormat = lvn_getLvnFormatEnum(swapchainData->swapchainFormat);
    swapchain->extent.width = swapchainData->swapchainExtent.width;
    swapchain->extent.height = swapchainData->swapchainExtent.height;

    return Lvn_Result_Success;

fail_cleanup:
    if (swapchainData)
    {
        vkBackends->vkDestroySwapchainKHR(vkBackends->device, swapchainData->swapchain, NULL);

        for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
        {
            if (swapchainImages[i].textureData)
                lvn_free(swapchainImages[i].textureData);

            vkBackends->vkDestroyImageView(vkBackends->device, swapchainData->swapchainImageViews[i], NULL);
        }

        lvn_free(swapchainImages);
        lvn_free(swapchainData->swapchainImages);
        lvn_free(swapchainData->swapchainImageViews);
        lvn_free(swapchainData);
    }
    return errResult;
}

void lvnImplVkDestroySwapchain(LvnSwapchain* swapchain)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) swapchain->graphicsctx->implData;
    LvnVkSwapchainData* swapchainData = (LvnVkSwapchainData*) swapchain->swapchainData;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    // swapchain image views
    for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
    {
        lvn_free(swapchain->pSwapchainImages[i].textureData);
        vkBackends->vkDestroyImageView(vkBackends->device, swapchainData->swapchainImageViews[i], NULL);
    }
    lvn_free(swapchainData->swapchainImageViews);

    // swapchain images
    lvn_free(swapchainData->swapchainImages);

    // swapchain
    vkBackends->vkDestroySwapchainKHR(vkBackends->device, swapchainData->swapchain, NULL);

    // swapchain data struct
    lvn_free(swapchain->swapchainData);
    lvn_free(swapchain->pSwapchainImages);
    swapchain->swapchainData = NULL;
    swapchain->pSwapchainImages = NULL;
    swapchain->swapchainImageCount = 0;
}

LvnResult lvnImplVkCreateRenderPass(const LvnGraphicsContext* graphicsctx, LvnRenderPass* renderpass, const LvnRenderPassCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && renderpass && createInfo, "graphicsctx, renderpass, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    LvnResult result = Lvn_Result_Failure;
    VkAttachmentDescription* attachments = NULL;
    VkAttachmentReference* colorAttachmentRefs = NULL;
    VkAttachmentReference* resolveAttachmentRefs = NULL;
    LvnVkRenderpassData* renderpassData = NULL;

    renderpassData = (LvnVkRenderpassData*) lvn_calloc(sizeof(LvnVkRenderpassData));
    if (!renderpassData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for renderpassData in renderpass %p",
                      renderpass);
        result = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

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
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for attachment descriptions array in renderpass %p",
                      renderpass);
        result = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }
    attachmentCount = 0;

    if (createInfo->colorAttachmentCount > 0)
    {
        colorAttachmentRefs = (VkAttachmentReference*) lvn_calloc(createInfo->colorAttachmentCount * sizeof(VkAttachmentReference));
        if (!colorAttachmentRefs)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to allocate memory for color attachment references array in renderpass %p",
                          renderpass);
            result = Lvn_Result_OutOfMemory;
            goto fail_cleanup;
        }
    }
    if (resolveCount > 0)
    {
        resolveAttachmentRefs = (VkAttachmentReference*) lvn_calloc(createInfo->colorAttachmentCount * sizeof(VkAttachmentReference));
        if (!resolveAttachmentRefs)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to allocate memory for resolve attachment reference array in renderpass %p",
                          renderpass);
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
    if (vkBackends->vkCreateRenderPass(vkBackends->device, &renderPassInfo, NULL, &vkRenderPass) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create vulkan renderpass in renderpass %p",
                      renderpass);
        goto fail_cleanup;
    }

    renderpassData->renderPass = vkRenderPass;
    renderpassData->hasDepthStencil = (createInfo->depthStencilAttachment) ? true : false;

    renderpass->renderpassData = renderpassData;

    lvn_free(resolveAttachmentRefs);
    lvn_free(colorAttachmentRefs);
    lvn_free(attachments);

    return Lvn_Result_Success;

fail_cleanup:
    lvn_free(resolveAttachmentRefs);
    lvn_free(colorAttachmentRefs);
    lvn_free(attachments);
    lvn_free(renderpassData);
    return result;
}

void lvnImplVkDestroyRenderPass(LvnRenderPass* renderpass)
{
    LVN_ASSERT(renderpass, "renderpass cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) renderpass->graphicsctx->implData;
    LvnVkRenderpassData* renderpassData = (LvnVkRenderpassData*) renderpass->renderpassData;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    vkBackends->vkDestroyRenderPass(vkBackends->device, renderpassData->renderPass, NULL);

    lvn_free(renderpassData);
    renderpass->renderpassData = NULL;
}

LvnResult lvnImplVkCreateFramebuffer(const LvnGraphicsContext* graphicsctx, LvnFramebuffer* framebuffer, const LvnFramebufferCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && framebuffer && createInfo, "graphicsctx, framebuffer, and createInfo cannot be null");

    LvnVulkanBackends* vkBackends = (LvnVulkanBackends*) graphicsctx->implData;
    LvnVkRenderpassData* renderpassData = (LvnVkRenderpassData*) createInfo->renderPass->renderpassData;
    LvnResult result = Lvn_Result_Failure;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

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
            LvnVkTextureData* colorTextureData = (LvnVkTextureData*) createInfo->pColorAttachments[i]->textureData;

            attachments[attachmentIndex++] = colorTextureData->imageView;
            if (createInfo->pResolveAttachments)
            {
                LvnVkTextureData* resolveTextureData = (LvnVkTextureData*) createInfo->pResolveAttachments[i]->textureData;
                attachments[attachmentIndex++] = resolveTextureData->imageView;
            }
        }

        if (createInfo->depthStencilAttachment)
        {
            LvnVkTextureData* depthTextureData = (LvnVkTextureData*) createInfo->depthStencilAttachment->textureData;
            attachments[attachmentIndex++] = depthTextureData->imageView;
        }
    }

    VkFramebufferCreateInfo framebufferInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderpassData->renderPass,
        .attachmentCount = attachmentCount,
        .pAttachments = attachments,
        .width = createInfo->width,
        .height = createInfo->height,
        .layers = 1,
    };

    VkFramebuffer vkFramebuffer;
    if (vkBackends->vkCreateFramebuffer(vkBackends->device, &framebufferInfo, NULL, &vkFramebuffer) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create framebuffer");
        goto fail_cleanup;
    }

    framebuffer->framebufferData = vkFramebuffer;

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

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    VkFramebuffer vkFramebuffer = (VkFramebuffer) framebuffer->framebufferData;

    vkBackends->vkDestroyFramebuffer(vkBackends->device, vkFramebuffer, NULL);
    framebuffer->framebufferData = NULL;
}

LvnResult lvnImplVkCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && shader && createInfo, "graphicsctx, shader, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnVkShaderData* shaderData = NULL;

    shaderData = (LvnVkShaderData*) lvn_calloc(sizeof(LvnVkShaderData));
    if (!shaderData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for shader data in shader %p",
                      shader);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    // entry point string
    size_t entryPointStrLen = strlen(createInfo->entryPoint) + 1;
    shaderData->entryPoint = (char*) lvn_calloc(entryPointStrLen);
    if (!shaderData->entryPoint)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for entryPoint string in shader data in shader %p",
                      shader);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }
    memcpy(shaderData->entryPoint, createInfo->entryPoint, entryPointStrLen);

    // shader module
    VkShaderModuleCreateInfo shaderCreateInfo = {0};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = createInfo->codeSize;
    shaderCreateInfo.pCode = (const uint32_t*) createInfo->pCode;

    if (vkBackends->vkCreateShaderModule(vkBackends->device, &shaderCreateInfo, NULL, &shaderData->shaderModule) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create shader module in shader %p",
                      shader);
        goto fail_cleanup;
    }

    shaderData->shaderStage = lvn_getVkShaderStageEnum(createInfo->stage);

    shader->shaderData = shaderData;

    return Lvn_Result_Success;

fail_cleanup:
    if (shaderData)
    {
        vkBackends->vkDestroyShaderModule(vkBackends->device, shaderData->shaderModule, NULL);
        lvn_free(shaderData->entryPoint);
        lvn_free(shaderData);
    }
    return errResult;
}

void lvnImplVkDestroyShader(LvnShader* shader)
{
    LVN_ASSERT(shader, "shader cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) shader->graphicsctx->implData;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    LvnVkShaderData* shaderData = (LvnVkShaderData*) shader->shaderData;

    vkBackends->vkDestroyShaderModule(vkBackends->device, shaderData->shaderModule, NULL);
    lvn_free(shaderData->entryPoint);
    lvn_free(shaderData);

    shader->shaderData = NULL;
}

LvnResult lvnImplVkCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && pipeline && createInfo, "graphicsctx, pipeline, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    VkPipelineShaderStageCreateInfo* shaderStages = NULL;
    VkVertexInputBindingDescription* bindingDescriptions = NULL;
    VkVertexInputAttributeDescription* vertexAttributes = NULL;
    VkDescriptorSetLayout* descriptorLayouts = NULL;
    VkFormat* colorAttachmentFormats = NULL;
    VkPipelineColorBlendAttachmentState* colorBlendAttachments = NULL;
    LvnVkPipelineData* pipelineData = NULL;

    pipelineData = (LvnVkPipelineData*) lvn_calloc(sizeof(LvnVkPipelineData));
    if (!pipelineData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for pipeline data in pipeline %p",
                      pipeline);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    // shader stages
    shaderStages = (VkPipelineShaderStageCreateInfo*)
        lvn_calloc(createInfo->stageCount * sizeof(VkPipelineShaderStageCreateInfo));
    if (createInfo->stageCount && !shaderStages)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for shader stages array in pipeline %p",
                      pipeline);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->stageCount; i++)
    {
        const LvnVkShaderData* shaderData = (const LvnVkShaderData*) createInfo->pShaderStages[i]->shaderData;

        VkPipelineShaderStageCreateInfo stageCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = shaderData->shaderStage,
            .module = shaderData->shaderModule,
            .pName = shaderData->entryPoint,
        };

        shaderStages[i] = stageCreateInfo;
    }

    // vertex binding descriptions
    bindingDescriptions = (VkVertexInputBindingDescription*)
        lvn_calloc(createInfo->vertexBindingDescriptionCount * sizeof(VkVertexInputBindingDescription));
    if (createInfo->vertexBindingDescriptionCount && !bindingDescriptions)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for binding descriptions array in pipeline %p",
                      pipeline);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->vertexBindingDescriptionCount; i++)
    {
        VkVertexInputBindingDescription bindingDescription = {
            .binding = createInfo->pVertexBindingDescriptions[i].binding,
            .stride = createInfo->pVertexBindingDescriptions[i].stride,
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };

        bindingDescriptions[i] = bindingDescription;
    }

    // vertex attributes
    vertexAttributes = (VkVertexInputAttributeDescription*)
        lvn_calloc(createInfo->vertexAttributeCount * sizeof(VkVertexInputAttributeDescription));
    if (createInfo->vertexAttributeCount && !vertexAttributes)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for vertex attributes array in pipeline %p",
                      pipeline);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->vertexAttributeCount; i++)
    {
        VkVertexInputAttributeDescription attributeDescription = {
            .binding = createInfo->pVertexAttributes[i].binding,
            .location = createInfo->pVertexAttributes[i].layout,
            .format = lvn_getVkVertexAttributeFormatEnum(createInfo->pVertexAttributes[i].format),
            .offset = createInfo->pVertexAttributes[i].offset,
        };

        vertexAttributes[i] = attributeDescription;
    }

    // send binding descriptions and attributes to pipeline
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

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
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for descriptor layouts array in pipeline %p",
                      pipeline);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->descriptorLayoutCount; i++)
    {
        VkDescriptorSetLayout descriptorLayout = (VkDescriptorSetLayout) createInfo->pDescriptorLayouts[i]->descriptorLayoutData;
        descriptorLayouts[i] = descriptorLayout;
    }

    // pipeline fixed functions
    const LvnPipelineFixedFunctions* pipelineFixedFunctions = createInfo->pipelineFixedFunctions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = lvn_getVkTopologyTypeEnum(pipelineFixedFunctions->inputAssembly.topology),
        .primitiveRestartEnable = pipelineFixedFunctions->inputAssembly.primitiveRestartEnable,
    };

    VkDynamicState dynamicStates[5];
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
    uint32_t dynamicStatesCount = 2;

    if (pipelineFixedFunctions->depthstencil.stencilTestEnable)
    {
        dynamicStates[2] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;
        dynamicStates[3] = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
        dynamicStates[4] = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
        dynamicStatesCount = 5;
    }

    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pDynamicStates = dynamicStates,
        .dynamicStateCount = dynamicStatesCount,
    };

    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = pipelineFixedFunctions->rasterizer.depthClampEnable,
        .rasterizerDiscardEnable = pipelineFixedFunctions->rasterizer.rasterizerDiscardEnable,
        .polygonMode = lvn_getVkPolygonModeEnum(pipelineFixedFunctions->rasterizer.polygonMode),
        .lineWidth = pipelineFixedFunctions->rasterizer.lineWidth,
        .cullMode = lvn_getVkCullModeFlagEnum(pipelineFixedFunctions->rasterizer.cullMode),
        .frontFace = lvn_getVkCullFrontFaceEnum(pipelineFixedFunctions->rasterizer.frontFace),
        .depthBiasEnable = pipelineFixedFunctions->rasterizer.depthBiasEnable,
        .depthBiasConstantFactor = pipelineFixedFunctions->rasterizer.depthBiasConstantFactor,
        .depthBiasClamp = pipelineFixedFunctions->rasterizer.depthBiasClamp,
        .depthBiasSlopeFactor = pipelineFixedFunctions->rasterizer.depthBiasSlopeFactor,
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = pipelineFixedFunctions->multisampling.sampleShadingEnable,
        .rasterizationSamples = lvn_getVkSampleCountFlagEnum(pipelineFixedFunctions->multisampling.rasterizationSamples),
        .minSampleShading = pipelineFixedFunctions->multisampling.minSampleShading,
        .pSampleMask = pipelineFixedFunctions->multisampling.sampleMask,
        .alphaToCoverageEnable = pipelineFixedFunctions->multisampling.alphaToCoverageEnable,
        .alphaToOneEnable = pipelineFixedFunctions->multisampling.alphaToOneEnable,
    };

    // if color blend attachments is 0, automatically add a default color blend attachment
    uint32_t colorBlendAttachmentCount = (pipelineFixedFunctions->colorBlend.colorBlendAttachmentCount == 0)
        ? 1
        : pipelineFixedFunctions->colorBlend.colorBlendAttachmentCount;

    colorBlendAttachments = (VkPipelineColorBlendAttachmentState*)
        lvn_calloc(colorBlendAttachmentCount * sizeof(VkPipelineColorBlendAttachmentState));

    if (colorBlendAttachmentCount && !colorBlendAttachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for color blend attachments array in pipeline %p",
                      pipeline);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    if (pipelineFixedFunctions->colorBlend.colorBlendAttachmentCount == 0)
    {
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
        };

        colorBlendAttachments[0] = colorBlendAttachment;
    }
    else
    {
        for (uint32_t i = 0; i < colorBlendAttachmentCount; i++)
        {
            LvnPipelineColorBlendAttachment attachment = pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i];

            VkPipelineColorBlendAttachmentState colorBlendAttachment = {
                .colorWriteMask = lvn_getVkColorComponentsFlagEnum(attachment.colorWriteMask),
                .blendEnable = attachment.blendEnable,
                .srcColorBlendFactor = lvn_getVkBlendFactorEnum(attachment.srcColorBlendFactor),
                .dstColorBlendFactor = lvn_getVkBlendFactorEnum(attachment.dstColorBlendFactor),
                .colorBlendOp = lvn_getVkBlendOperationEnum(attachment.colorBlendOp),
                .srcAlphaBlendFactor = lvn_getVkBlendFactorEnum(attachment.srcAlphaBlendFactor),
                .dstAlphaBlendFactor = lvn_getVkBlendFactorEnum(attachment.dstAlphaBlendFactor),
                .alphaBlendOp = lvn_getVkBlendOperationEnum(attachment.alphaBlendOp),
            };

            colorBlendAttachments[i] = colorBlendAttachment;
        }
    }

    VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = pipelineFixedFunctions->colorBlend.logicOpEnable,
        .logicOp = lvn_getVkLogicOpEnum(pipelineFixedFunctions->colorBlend.logicOp),
        .pAttachments = colorBlendAttachments,
        .attachmentCount = colorBlendAttachmentCount,
        .blendConstants[0] = pipelineFixedFunctions->colorBlend.blendConstants[0],
        .blendConstants[1] = pipelineFixedFunctions->colorBlend.blendConstants[1],
        .blendConstants[2] = pipelineFixedFunctions->colorBlend.blendConstants[2],
        .blendConstants[3] = pipelineFixedFunctions->colorBlend.blendConstants[3],
    };

    VkPipelineDepthStencilStateCreateInfo depthStencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = pipelineFixedFunctions->depthstencil.depthTestEnable,
        .depthWriteEnable = pipelineFixedFunctions->depthstencil.depthWriteEnable,
        .depthCompareOp = lvn_getVkCompareOpEnum(pipelineFixedFunctions->depthstencil.depthOpCompare),
        .depthBoundsTestEnable = VK_FALSE,
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
        .stencilTestEnable = pipelineFixedFunctions->depthstencil.stencilTestEnable,
        .back.compareMask = pipelineFixedFunctions->depthstencil.stencil.compareMask,
        .back.writeMask = pipelineFixedFunctions->depthstencil.stencil.writeMask,
        .back.reference = pipelineFixedFunctions->depthstencil.stencil.reference,
        .back.compareOp = lvn_getVkCompareOpEnum(pipelineFixedFunctions->depthstencil.stencil.compareOp),
        .back.depthFailOp = lvn_getVkStencilOpEnum(pipelineFixedFunctions->depthstencil.stencil.depthFailOp),
        .back.failOp = lvn_getVkStencilOpEnum(pipelineFixedFunctions->depthstencil.stencil.failOp),
        .back.passOp = lvn_getVkStencilOpEnum(pipelineFixedFunctions->depthstencil.stencil.passOp),
        .front = depthStencil.back,
    };

    // pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

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

    if (vkBackends->vkCreatePipelineLayout(vkBackends->device, &pipelineLayoutInfo, NULL, &pipelineData->pipelineLayout) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create pipeline layout for pipeline %p",
                      pipeline);
        goto fail_cleanup;
    }

    LvnVkRenderpassData* renderpassData = (LvnVkRenderpassData*) createInfo->renderPass->renderpassData;

    // pipeline create info
    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = createInfo->stageCount,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = pipelineData->pipelineLayout,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
        .renderPass = renderpassData->renderPass,
    };

    if (vkBackends->vkCreateGraphicsPipelines(vkBackends->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipelineData->pipeline) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create graphics pipeline for pipeline %p",
                      pipeline);
        goto fail_cleanup;
    }

    pipeline->pipelineData = pipelineData;

    lvn_free(colorBlendAttachments);
    lvn_free(colorAttachmentFormats);
    lvn_free(descriptorLayouts);
    lvn_free(vertexAttributes);
    lvn_free(bindingDescriptions);
    lvn_free(shaderStages);

    return Lvn_Result_Success;

fail_cleanup:
    if (pipelineData)
    {
        vkBackends->vkDestroyPipeline(vkBackends->device, pipelineData->pipeline, NULL);
        vkBackends->vkDestroyPipelineLayout(vkBackends->device, pipelineData->pipelineLayout, NULL);
        lvn_free(pipelineData);
    }
    lvn_free(colorBlendAttachments);
    lvn_free(colorAttachmentFormats);
    lvn_free(descriptorLayouts);
    lvn_free(vertexAttributes);
    lvn_free(bindingDescriptions);
    lvn_free(shaderStages);
    return errResult;
}

void lvnImplVkDestroyPipeline(LvnPipeline* pipeline)
{
    LVN_ASSERT(pipeline, "pipeline cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) pipeline->graphicsctx->implData;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    LvnVkPipelineData* pipelineData = (LvnVkPipelineData*) pipeline->pipelineData;

    vkBackends->vkDestroyPipeline(vkBackends->device, pipelineData->pipeline, NULL);
    vkBackends->vkDestroyPipelineLayout(vkBackends->device, pipelineData->pipelineLayout, NULL);

    lvn_free(pipelineData);

    pipeline->pipelineData = NULL;
}

LvnResult lvnImplVkCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence* fence, bool signaled)
{
    LVN_ASSERT(graphicsctx && fence, "graphicsctx and fence cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = (signaled) ? VK_FENCE_CREATE_SIGNALED_BIT : 0,
    };

    VkFence vkFence;
    if (vkBackends->vkCreateFence(vkBackends->device, &fenceInfo, NULL, &vkFence) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create VkFence");
        return Lvn_Result_Failure;
    }

    fence->fenceData = vkFence;

    return Lvn_Result_Success;
}

void lvnImplVkDestroyFence(LvnFence* fence)
{
    LVN_ASSERT(fence, "fence cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) fence->graphicsctx->implData;
    vkBackends->vkDeviceWaitIdle(vkBackends->device);
    VkFence vkFence = (VkFence) fence->fenceData;
    vkBackends->vkDestroyFence(vkBackends->device, vkFence, NULL);
}

LvnResult lvnImplVkCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore* semaphore)
{
    LVN_ASSERT(graphicsctx && semaphore, "graphicsctx and semaphore cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkSemaphore vkSemaphore;
    if (vkBackends->vkCreateSemaphore(vkBackends->device, &semaphoreInfo, NULL, &vkSemaphore) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to create VkSemaphore");
        return Lvn_Result_Failure;
    }

    semaphore->semaphoreData = vkSemaphore;
    return Lvn_Result_Success;
}

void lvnImplVkDestroySemaphore(LvnSemaphore* semaphore)
{
    LVN_ASSERT(semaphore, "semaphore cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) semaphore->graphicsctx->implData;
    vkBackends->vkDeviceWaitIdle(vkBackends->device);
    VkSemaphore vkSemaphore = (VkSemaphore) semaphore->semaphoreData;
    vkBackends->vkDestroySemaphore(vkBackends->device, vkSemaphore, NULL);
}

LvnResult lvnImplVkCreateBuffer(const LvnGraphicsContext* graphicsctx, LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && buffer && createInfo, "graphicsctx, buffer, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnVkBufferData* bufferData = NULL;
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VmaAllocation stagingMemory = VK_NULL_HANDLE;
    VkBuffer vkBuffer = VK_NULL_HANDLE;
    VmaAllocation bufferMemory = VK_NULL_HANDLE;


    bufferData = (LvnVkBufferData*) lvn_calloc(sizeof(LvnVkBufferData));
    if (!bufferData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for buffer data in buffer %p",
                      buffer);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    bufferData->usageFlags = lvn_getVkBufferUsageFlagsEnum(createInfo->type);

    if (createInfo->usage == Lvn_BufferMemoryUsage_GpuOnly)
    {
        // staging buffer
        LvnResult result = lvn_createBuffer(vkBackends, &stagingBuffer, &stagingMemory, createInfo->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
        if (result != Lvn_Result_Success)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to create vulkan staging buffer object in buffer %p",
                          buffer);
            errResult = result;
            goto fail_cleanup;
        }

        if (createInfo->data)
        {
            void* data;
            vmaMapMemory(vkBackends->vmaAllocator, stagingMemory, &data);
            memcpy(data, createInfo->data, createInfo->size);
            vmaUnmapMemory(vkBackends->vmaAllocator, stagingMemory);
        }

        // gpu only buffer
        bufferData->usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        result = lvn_createBuffer(vkBackends, &vkBuffer, &bufferMemory, createInfo->size, bufferData->usageFlags, VMA_MEMORY_USAGE_GPU_ONLY);
        if (result != Lvn_Result_Success)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to create static vulkan buffer object in buffer %p",
                          buffer);
            errResult = result;
            goto fail_cleanup;
        }

        lvn_copyBuffer(vkBackends, stagingBuffer, vkBuffer, createInfo->size, 0, 0);

        vkBackends->vkDestroyBuffer(vkBackends->device, stagingBuffer, NULL);
        vmaFreeMemory(vkBackends->vmaAllocator, stagingMemory);

        bufferData->buffer = vkBuffer;
        bufferData->bufferMemory = bufferMemory;
    }
    else
    {
        VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;
        if (createInfo->usage == Lvn_BufferMemoryUsage_CpuToGpu)
            memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        else if (createInfo->usage == Lvn_BufferMemoryUsage_GpuToCpu)
            memoryUsage = VMA_MEMORY_USAGE_GPU_TO_CPU;

        LvnResult result = lvn_createBuffer(vkBackends, &vkBuffer, &bufferMemory, createInfo->size, bufferData->usageFlags, memoryUsage);
        if (result != Lvn_Result_Success)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to create dynamic vulkan buffer object in buffer %p",
                          buffer);
            errResult = result;
            goto fail_cleanup;
        }

        vmaMapMemory(vkBackends->vmaAllocator, bufferMemory, &bufferData->bufferMap);
        if (createInfo->data)
            memcpy(bufferData->bufferMap, createInfo->data, createInfo->size);

        bufferData->buffer = vkBuffer;
        bufferData->bufferMemory = bufferMemory;
    }

    buffer->bufferData = bufferData;

    return Lvn_Result_Success;

fail_cleanup:
    vkBackends->vkDestroyBuffer(vkBackends->device, stagingBuffer, NULL);
    vkBackends->vkDestroyBuffer(vkBackends->device, vkBuffer, NULL);
    vmaFreeMemory(vkBackends->vmaAllocator, stagingMemory);
    vmaFreeMemory(vkBackends->vmaAllocator, bufferMemory);
    lvn_free(bufferData);
    return errResult;
}

void lvnImplVkDestroyBuffer(LvnBuffer* buffer)
{
    LVN_ASSERT(buffer, "buffer cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) buffer->graphicsctx->implData;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    LvnVkBufferData* bufferData = (LvnVkBufferData*) buffer->bufferData;

    if (buffer->usage != Lvn_BufferMemoryUsage_GpuOnly)
        vmaUnmapMemory(vkBackends->vmaAllocator, bufferData->bufferMemory);

    vkBackends->vkDestroyBuffer(vkBackends->device, bufferData->buffer, NULL);
    vmaFreeMemory(vkBackends->vmaAllocator, bufferData->bufferMemory);

    lvn_free(bufferData);

    buffer->bufferData = NULL;
}

LvnResult lvnImplVkCreateSampler(const LvnGraphicsContext* graphicsctx, LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo)
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
    vkBackends->vkGetPhysicalDeviceFeatures(vkBackends->physicalDevice, &physicalDeviceFeatures);

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkBackends->vkGetPhysicalDeviceProperties(vkBackends->physicalDevice, &physicalDeviceProperties);

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
    if (vkBackends->vkCreateSampler(vkBackends->device, &samplerInfo, NULL, &textureSampler) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create sampler <VkSampler> (%p)",
                      textureSampler);
        return Lvn_Result_Failure;
    }

    sampler->samplerData = textureSampler;

    return Lvn_Result_Success;
}

void lvnImplVkDestroySampler(LvnSampler* sampler)
{
    LVN_ASSERT(sampler, "sampler cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) sampler->graphicsctx->implData;
    vkBackends->vkDeviceWaitIdle(vkBackends->device);
    VkSampler textureSampler = (VkSampler) sampler->samplerData;
    vkBackends->vkDestroySampler(vkBackends->device, textureSampler, NULL);
}

LvnResult lvnImplVkCreateTexture(const LvnGraphicsContext* graphicsctx, LvnTexture* texture, const LvnTextureCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && texture && createInfo, "graphicsctx, texture, and createInfo cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    LvnResult errResult = Lvn_Result_Failure;

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VmaAllocation stagingBufferMemory = VK_NULL_HANDLE;
    VkImage textureImage = VK_NULL_HANDLE;
    VmaAllocation textureImageMemory = VK_NULL_HANDLE;
    LvnVkTextureData* textureData = NULL;

    textureData = (LvnVkTextureData*) lvn_calloc(sizeof(LvnVkTextureData));
    if (!textureData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to allocate memory for texture data in texture (%p)",
                      texture);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    VkDeviceSize imageSize = createInfo->width * createInfo->height * createInfo->image->channels;
    if (lvn_createBuffer(vkBackends, &stagingBuffer, &stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create transision buffer <VkBuffer> for image <VkImage> for texture (%p)",
                      texture);
        goto fail_cleanup;
    }

    void* data;
    vmaMapMemory(vkBackends->vmaAllocator, stagingBufferMemory, &data);
    memcpy(data, createInfo->image->data, imageSize);
    vmaUnmapMemory(vkBackends->vmaAllocator, stagingBufferMemory);

    VkFormat format = lvn_getVkFormatEnum(createInfo->format);

    // create texture image
    if (lvn_createImage(vkBackends,
        &textureImage,
        &textureImageMemory,
        createInfo->width,
        createInfo->height,
        format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        lvn_getVkSampleCountFlagEnum(createInfo->samples),
        VMA_MEMORY_USAGE_GPU_ONLY) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create texture image <VkImage> for texture (%p)",
                      texture);
        goto fail_cleanup;
    }

    // transition buffer to image
    lvn_transitionImageLayout(vkBackends, textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
    lvn_copyBufferToImage(vkBackends, stagingBuffer, textureImage, createInfo->width, createInfo->height, 1);
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
    if (vkBackends->vkCreateImageView(vkBackends->device, &viewInfo, NULL, &imageView) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to create texture image view <VkImageView> for texture (%p)",
                      texture);
        goto fail_cleanup;
    }

    textureData->image = textureImage;
    textureData->imageView = imageView;
    textureData->imageMemory = textureImageMemory;
    textureData->sampler = createInfo->sampler->samplerData;

    texture->textureData = textureData;

    vkBackends->vkDestroyBuffer(vkBackends->device, stagingBuffer, NULL);
    vmaFreeMemory(vkBackends->vmaAllocator, stagingBufferMemory);

    return Lvn_Result_Success;

fail_cleanup:
    vkBackends->vkDestroyImage(vkBackends->device, textureImage, NULL);
    vkBackends->vkDestroyBuffer(vkBackends->device, stagingBuffer, NULL);
    vmaFreeMemory(vkBackends->vmaAllocator, stagingBufferMemory);
    vmaFreeMemory(vkBackends->vmaAllocator, textureImageMemory);
    lvn_free(textureData);
    return errResult;
}

void lvnImplVkDestroyTexture(LvnTexture* texture)
{
    LVN_ASSERT(texture, "texture cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) texture->graphicsctx->implData;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    LvnVkTextureData* textureData = (LvnVkTextureData*) texture->textureData;

    vkBackends->vkDestroyImage(vkBackends->device, textureData->image, NULL);
    vmaFreeMemory(vkBackends->vmaAllocator, textureData->imageMemory);
    vkBackends->vkDestroyImageView(vkBackends->device, textureData->imageView, NULL);

    lvn_free(textureData);

    texture->textureData = NULL;
}

LvnResult lvnImplVkCreateCommandBuffer(const LvnGraphicsContext* graphicsctx, LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(graphicsctx && commandBuffer, "graphicsctx and commandBuffer cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;

    VkCommandBufferAllocateInfo cmdBufferAllocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
        .commandPool = vkBackends->commandPool,
    };

    VkCommandBuffer vkCommandBuffer;
    if (vkBackends->vkAllocateCommandBuffers(vkBackends->device, &cmdBufferAllocInfo, &vkCommandBuffer) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger, "[vulkan] failed to allocate command buffer");
        return Lvn_Result_Failure;
    }

    commandBuffer->commandbufferData = vkCommandBuffer;

    return Lvn_Result_Success;
}

void lvnImplVkDestroyCommandBuffer(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    VkCommandBuffer vkCommandBuffer = (VkCommandBuffer) commandBuffer->commandbufferData;
    vkBackends->vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &vkCommandBuffer);

    commandBuffer->commandbufferData = NULL;
}

void lvnImplVkSurfaceGetSupportedFormats(const LvnSurface* surface, uint32_t* formatCount, LvnFormat* pSurfaceFormats)
{
    LVN_ASSERT(surface && formatCount, "surface and formatCount cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) surface->graphicsctx->implData;
    VkSurfaceKHR vkSurface = (VkSurfaceKHR) surface->surfaceData;

    uint32_t vkFormatCount = 0;
    vkBackends->vkGetPhysicalDeviceSurfaceFormatsKHR(vkBackends->physicalDevice, vkSurface, &vkFormatCount, NULL);

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

    vkBackends->vkGetPhysicalDeviceSurfaceFormatsKHR(vkBackends->physicalDevice, vkSurface, &vkFormatCount, formats);

    uint32_t supportedFormatCount = 0;
    for (uint32_t i = 0; i < vkFormatCount; i++)
    {
        // NOTE: support other colorspace in future?
        if (formats[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            continue;

        LvnFormat format = lvn_getLvnFormatEnum(formats[i].format);
        if (format != Lvn_Format_Undefined)
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
    VkSurfaceKHR vkSurface = (VkSurfaceKHR) surface->surfaceData;

    uint32_t vkPresentModeCount = 0;
    vkBackends->vkGetPhysicalDeviceSurfacePresentModesKHR(vkBackends->physicalDevice, vkSurface, &vkPresentModeCount, NULL);

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

    vkBackends->vkGetPhysicalDeviceSurfacePresentModesKHR(vkBackends->physicalDevice, vkSurface, &vkPresentModeCount, presentModes);

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

    vkBackends->vkDeviceWaitIdle(vkBackends->device);

    LvnResult errResult = Lvn_Result_Failure;
    LvnVkSwapchainData* swapchainData = (LvnVkSwapchainData*) swapchain->swapchainData;
    VkSurfaceKHR surface = (VkSurfaceKHR) swapchainData->surface;

    // destroy swapchain resources
    for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
    {
        lvn_free(swapchain->pSwapchainImages[i].textureData);
        vkBackends->vkDestroyImageView(vkBackends->device, swapchainData->swapchainImageViews[i], NULL);
    }
    lvn_free(swapchainData->swapchainImages);
    lvn_free(swapchainData->swapchainImageViews);
    swapchainData->swapchainImages = NULL;
    swapchainData->swapchainImageViews = NULL;
    memset(swapchain->pSwapchainImages, 0, swapchain->swapchainImageCount * sizeof(LvnTexture));

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
                      "[vulkan] failed to recreate swapchain %p",
                      swapchain);
        return Lvn_Result_Failure;
    }

    // destroy old swapchain
    vkBackends->vkDestroySwapchainKHR(vkBackends->device, swapchainData->oldSwapchain, NULL);
    swapchainData->oldSwapchain = VK_NULL_HANDLE;

    if (swapchain->swapchainImageCount != swapchainData->swapchainImageCount)
    {
        swapchain->swapchainImageCount = swapchainData->swapchainImageCount;
        swapchain->pSwapchainImages = lvn_realloc(swapchain->pSwapchainImages, swapchainData->swapchainImageCount * sizeof(LvnTexture*));
    }

    // update image views
    for (uint32_t i = 0; i < swapchainData->swapchainImageCount; i++)
    {
        swapchain->pSwapchainImages[i].textureData = lvn_calloc(sizeof(LvnVkTextureData));
        if (!swapchain->pSwapchainImages[i].textureData)
        {
            LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger,
                          "[vulkan] failed to allocate memory for texture data for image in swapchain %p",
                          swapchain);
            errResult = Lvn_Result_OutOfMemory;
            goto fail_cleanup;
        }

        LvnVkTextureData* textureData = (LvnVkTextureData*) swapchain->pSwapchainImages[i].textureData;

        textureData->image = swapchainData->swapchainImages[i];
        textureData->imageView = swapchainData->swapchainImageViews[i];
        textureData->width = swapchainData->swapchainExtent.width;
        textureData->height = swapchainData->swapchainExtent.height;
    }

    // update extent
    swapchain->extent.width = swapchainData->swapchainExtent.width;
    swapchain->extent.height = swapchainData->swapchainExtent.height;

    return Lvn_Result_Success;

fail_cleanup:
    return errResult;
}

LvnResult lvnImplVkSwapchainAcquireNextImage(LvnSwapchain* swapchain, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex)
{
    LVN_ASSERT(swapchain && imageIndex, "swapchain and imageIndex cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) swapchain->graphicsctx->implData;
    VkSwapchainKHR vkSwapchain = ((LvnVkSwapchainData*)swapchain->swapchainData)->swapchain;
    VkSemaphore vkSemaphore = (semaphore != NULL) ? (VkSemaphore) semaphore->semaphoreData : VK_NULL_HANDLE;
    VkFence vkFence = (fence != NULL) ? (VkFence) fence->fenceData : VK_NULL_HANDLE;

    VkResult result = vkBackends->vkAcquireNextImageKHR(vkBackends->device, vkSwapchain, UINT64_MAX, vkSemaphore, vkFence, imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
        return Lvn_Result_OutOfDate;

    return (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) ? Lvn_Result_Success : Lvn_Result_Failure;
}

LvnResult lvnImplVkFenceWait(LvnFence* fence, uint64_t timeout)
{
    LVN_ASSERT(fence, "fence cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) fence->graphicsctx->implData;
    VkFence vkFence = (VkFence) fence->fenceData;
    return (vkBackends->vkWaitForFences(vkBackends->device, 1, &vkFence, VK_TRUE, timeout) == VK_SUCCESS)
        ? Lvn_Result_Success
        : Lvn_Result_TimeOut;
}

LvnResult lvnImplVkFenceReset(LvnFence* fence)
{
    LVN_ASSERT(fence, "fence cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) fence->graphicsctx->implData;
    VkFence vkFence = (VkFence) fence->fenceData;
    return (vkBackends->vkResetFences(vkBackends->device, 1, &vkFence) == VK_SUCCESS)
        ? Lvn_Result_Success
        : Lvn_Result_Failure;
}

void lvnImplVkBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset)
{
    LVN_ASSERT(buffer, "buffer cannot be null");
    LvnVkBufferData* bufferData = (LvnVkBufferData*) buffer->bufferData;
    memcpy((uint8_t*)bufferData->bufferMap + offset, data, size);
}

void lvnImplVkBufferResize(LvnBuffer* buffer, uint64_t size)
{

}

void lvnImplVkBeginCommandBuffer(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");

    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;

    VkCommandBufferBeginInfo cmdBuffBeginInfo = {0};
    cmdBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBuffBeginInfo.flags = 0;
    cmdBuffBeginInfo.pInheritanceInfo = NULL;

    if (vkBackends->vkBeginCommandBuffer(cmdBuff, &cmdBuffBeginInfo) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(vkBackends->graphicsctx->coreLogger, "[vulkan] failed to begin command buffer");
    }
}

void lvnImplVkEndCommandBuffer(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;
    vkBackends->vkEndCommandBuffer(cmdBuff);
}

void lvnImplVkCmdBeginRenderPass(LvnCommandBuffer* commandBuffer, LvnRenderPassBeginInfo* beginInfo)
{
    LVN_ASSERT(commandBuffer && beginInfo, "commandBuffer and beginInfo cannot be null");
    const LvnGraphicsContext* graphicsctx = (const LvnGraphicsContext*) commandBuffer->graphicsctx;
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;
    LvnVkRenderpassData* renderpassData = (LvnVkRenderpassData*) beginInfo->renderPass->renderpassData;
    VkFramebuffer framebuffer = (VkFramebuffer) beginInfo->framebuffer->framebufferData;

    LvnArenaMark mark = lvn_memArenaMark(&commandBuffer->frameArena);

    VkRect2D renderArea = {
        .offset = { beginInfo->renderArea.offset.x, beginInfo->renderArea.offset.y },
        .extent = { beginInfo->renderArea.extent.width, beginInfo->renderArea.extent.height },
    };

    uint32_t clearValueCount = beginInfo->clearColorValueCount + (renderpassData->hasDepthStencil ? 1 : 0);
    VkClearValue* clearColors = (VkClearValue*)
        lvn_memArenaAlloc(&commandBuffer->frameArena, clearValueCount * sizeof(VkClearValue));

    for (uint32_t i = 0; i < beginInfo->clearColorValueCount; i++)
        memcpy(&clearColors[i], &beginInfo->pClearColorValues[i], sizeof(VkClearValue));

    if (renderpassData->hasDepthStencil)
    {
        clearColors[clearValueCount - 1].depthStencil =
            (VkClearDepthStencilValue){ beginInfo->clearDepthStencilValue.depth, beginInfo->clearDepthStencilValue.stencil };
    }

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderpassData->renderPass,
        .framebuffer = framebuffer,
        .renderArea = renderArea,
        .clearValueCount = clearValueCount,
        .pClearValues = clearColors,
    };

    vkBackends->vkCmdBeginRenderPass(cmdBuff, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    lvn_memArenaMarkRevert(&commandBuffer->frameArena, &mark);
}

void lvnImplVkCmdEndRenderPass(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;
    vkBackends->vkCmdEndRenderPass(cmdBuff);
}

void lvnImplVkCmdBindPipeline(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;
    LvnVkPipelineData* pipelineData = (LvnVkPipelineData*) pipeline->pipelineData;

    vkBackends->vkCmdBindPipeline(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData->pipeline);
}

void lvnImplVkCmdBindVertexBuffer(LvnCommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnGraphicsContext* graphicsctx = (const LvnGraphicsContext*) commandBuffer->graphicsctx;
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;

    LvnArenaMark mark = lvn_memArenaMark(&commandBuffer->frameArena);

    VkBuffer* buffers = (VkBuffer*)
        lvn_memArenaAlloc(&commandBuffer->frameArena, bindingCount * sizeof(VkBuffer));
    for (uint32_t i = 0; i < bindingCount; i++)
        buffers[i] = ((LvnVkBufferData*)pBuffers[i]->bufferData)->buffer;

    vkBackends->vkCmdBindVertexBuffers(cmdBuff, firstBinding, bindingCount, buffers, pOffsets);

    lvn_memArenaMarkRevert(&commandBuffer->frameArena, &mark);
}

void lvnImplVkCmdBindIndexBuffer(LvnCommandBuffer* commandBuffer, LvnBuffer* buffer, uint64_t offset)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;

    LvnVkBufferData* bufferData = (LvnVkBufferData*) buffer->bufferData;

   vkBackends->vkCmdBindIndexBuffer(cmdBuff, bufferData->buffer, offset, VK_INDEX_TYPE_UINT32);
}

void lvnImplVkCmdSetViewport(LvnCommandBuffer* commandBuffer, const LvnViewport* viewport)
{
    LVN_ASSERT(commandBuffer && viewport, "commandBuffer and viewport cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;

    VkViewport viewportInfo = {
        .x        = viewport->x,
        .y        = viewport->y,
        .width    = viewport->width,
        .height   = viewport->height,
        .minDepth = viewport->minDepth,
        .maxDepth = viewport->maxDepth,
    };

    vkBackends->vkCmdSetViewport(cmdBuff, 0, 1, &viewportInfo);
}

void lvnImplVkCmdSetScissor(LvnCommandBuffer* commandBuffer, const LvnRenderArea* scissor)
{
    LVN_ASSERT(commandBuffer && scissor, "commandBuffer and scissor cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;

    VkRect2D scissorInfo = {
        .extent = { scissor->extent.width, scissor->extent.height },
        .offset = { scissor->offset.x, scissor->offset.y },
    };

    vkBackends->vkCmdSetScissor(cmdBuff, 0, 1, &scissorInfo);
}

void lvnImplVkCmdDraw(LvnCommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;
    vkBackends->vkCmdDraw(cmdBuff, vertexCount, instanceCount, firstVertex, firstInstance);
}

void lvnImplVkCmdDrawIndexed(LvnCommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
    const LvnVulkanBackends* vkBackends = (const LvnVulkanBackends*) commandBuffer->graphicsctx->implData;
    VkCommandBuffer cmdBuff = (VkCommandBuffer) commandBuffer->commandbufferData;
    vkBackends->vkCmdDrawIndexed(cmdBuff, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

LvnResult lvnImplVkRenderSubmit(const LvnGraphicsContext* graphicsctx, const LvnSubmitInfo* pSubmits, uint32_t submitCount, LvnFence* fence)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    LvnVulkanBackends* vkBackends = (LvnVulkanBackends*) graphicsctx->implData;
    VkFence vkFence = fence ? (VkFence)fence->fenceData : VK_NULL_HANDLE;
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    LvnArenaMark mark = lvn_memArenaMark(&vkBackends->frameArena);

    // get count of all semaphores and command buffers
    uint32_t waitSemaphoreCount = 0, signalSemaphoreCount = 0, commandBufferCount = 0;
    for (uint32_t i = 0; i < submitCount; i++)
    {
        waitSemaphoreCount += pSubmits[i].waitSemaphoreCount;
        signalSemaphoreCount += pSubmits[i].signalSemaphoreCount;
        commandBufferCount += pSubmits[i].commandBufferCount;
    }

    // arrays to store semaphores and command buffers for submit infos
    VkSemaphore* waitSemaphores = (VkSemaphore*)
        lvn_memArenaAlloc(&vkBackends->frameArena, waitSemaphoreCount * sizeof(VkSemaphore));
    VkSemaphore* signalSemaphores = (VkSemaphore*)
        lvn_memArenaAlloc(&vkBackends->frameArena, signalSemaphoreCount * sizeof(VkSemaphore));
    VkCommandBuffer* commandBuffers = (VkCommandBuffer*)
        lvn_memArenaAlloc(&vkBackends->frameArena, commandBufferCount * sizeof(VkCommandBuffer));
    VkSubmitInfo* submitInfos = (VkSubmitInfo*)
        lvn_memArenaAlloc(&vkBackends->frameArena, submitCount * sizeof(VkSubmitInfo));
    memset(submitInfos, 0, submitCount * sizeof(VkSubmitInfo));

    uint32_t waitSemaphoreOffset = 0, signalSemaphoreOffset = 0, commandBufferOffset = 0;

    for (uint32_t i = 0; i < submitCount; i++)
    {
        // get vulkan semaphore/command buffer handles
        for (uint32_t j = 0; j < pSubmits[i].waitSemaphoreCount; j++)
            waitSemaphores[waitSemaphoreOffset + j] = (VkSemaphore) pSubmits[i].pWaitSemaphores[j]->semaphoreData;

        for (uint32_t j = 0; j < pSubmits[i].signalSemaphoreCount; j++)
            signalSemaphores[signalSemaphoreOffset + j] = (VkSemaphore) pSubmits[i].pSignalSemaphores[j]->semaphoreData;

        for (uint32_t j = 0; j < pSubmits[i].commandBufferCount; j++)
            commandBuffers[commandBufferOffset + j] = (VkCommandBuffer) pSubmits[i].pCommandBuffers[j]->commandbufferData;

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
    if (vkBackends->vkQueueSubmit(vkBackends->graphicsQueue, submitCount, submitInfos, vkFence) != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to submit command buffers to queue");
        return Lvn_Result_Failure;
    }

    lvn_memArenaMarkRevert(&vkBackends->frameArena, &mark);

    return Lvn_Result_Success;
}

LvnResult lvnImplVkRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo)
{
    LVN_ASSERT(graphicsctx && presentInfo, "graphicsctx and presentInfo cannot be null");

    LvnVulkanBackends* vkBackends = (LvnVulkanBackends*) graphicsctx->implData;

    LvnArenaMark mark = lvn_memArenaMark(&vkBackends->frameArena);

    VkSemaphore* waitSemaphores = lvn_memArenaAlloc(&vkBackends->frameArena,
                                                    presentInfo->waitSemaphoreCount * sizeof(VkSemaphore));
    for (uint32_t i = 0; i < presentInfo->waitSemaphoreCount; i++)
        waitSemaphores[i] = (VkSemaphore) presentInfo->pWaitSemaphores[i]->semaphoreData;

    VkSwapchainKHR* swapchains = lvn_memArenaAlloc(&vkBackends->frameArena,
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

    VkResult result = vkBackends->vkQueuePresentKHR(vkBackends->presentQueue, &vkPresentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        return Lvn_Result_OutOfDate;
    else if (result != VK_SUCCESS)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to present swapchain image");
        return Lvn_Result_Failure;
    }

    lvn_memArenaMarkRevert(&vkBackends->frameArena, &mark);

    return Lvn_Result_Success;
}
