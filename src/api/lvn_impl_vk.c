#include "lvn_impl_vk.h"
#include "lvn_impl_vk_backends.h"

#include <string.h>

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

LvnResult lvnImplVkInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && createInfo, "graphicsctx and createInfo cannot be nullptr");

    LvnVulkanBackends* vkBackends = lvn_calloc(sizeof(LvnVulkanBackends));
    graphicsctx->implData = vkBackends;

    // load vulkan library
    vkBackends->handle = lvn_platformLoadModule(s_LvnVkLibName);

    if (!vkBackends->handle)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] unable to load vulkan shared library: %s",
                      s_LvnVkLibName);

        lvnImplVkTerminate(graphicsctx);
        return Lvn_Result_Failure;
    }

    vkBackends->getInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
        lvn_platformGetModuleSymbol(vkBackends->handle, "vkGetInstanceProcAddr");

    if (!vkBackends->getInstanceProcAddr)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] unable to retrieve vkGetInstanceProcAddr symbol");

        lvnImplVkTerminate(graphicsctx);
        return Lvn_Result_Failure;
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

        lvnImplVkTerminate(graphicsctx);
        return Lvn_Result_Failure;
    }


    // query vulkan instance exensions for surface support
    const char** extensionNames = NULL;
    uint32_t extensionCount = 0;

    if (createInfo->presentationModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        uint32_t count = 0;
        VkResult result = vkBackends->enumerateInstanceExtensionProperties(NULL, &count, NULL);
        if (result != VK_SUCCESS)
        {
            // NOTE: this happens on systems with a loader but without any vulkan ICD
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to query vulkan instance extensions");

            lvnImplVkTerminate(graphicsctx);
            return Lvn_Result_Failure;
        }

        VkExtensionProperties* extensionProps = lvn_calloc(count * sizeof(VkExtensionProperties));
        result = vkBackends->enumerateInstanceExtensionProperties(NULL, &count, extensionProps);
        if (result != VK_SUCCESS)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to query vulkan instance extensions");

            lvnImplVkTerminate(graphicsctx);
            return Lvn_Result_Failure;
        }

        for (uint32_t i = 0; i < count; i++)
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

        lvn_free(extensionProps);

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
    if (createInfo->enableGraphicsApiDebugLogging)
    {
        uint32_t layerCount;
        vkBackends->enumerateInstanceLayerProperties(&layerCount, NULL);

        VkLayerProperties* availableLayers = lvn_calloc(layerCount * sizeof(VkLayerProperties));
        vkBackends->enumerateInstanceLayerProperties(&layerCount, availableLayers);

        for (uint32_t i = 0; i < LVN_ARRAY_LEN(s_LvnVkValidationLayers); i++)
        {
            bool layerFound = false;
            for (uint32_t j = 0; j < layerCount; j++)
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

        lvn_free(availableLayers);
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

    if (createInfo->enableGraphicsApiDebugLogging)
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

        lvnImplVkTerminate(graphicsctx);
        return Lvn_Result_Failure;
    }

    lvn_free(extensionNames);

    // get post instance function symbols
    vkBackends->destroyInstance = (PFN_vkDestroyInstance)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkDestroyInstance");
    vkBackends->enumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkEnumeratePhysicalDevices");
    vkBackends->getPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
        vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceQueueFamilyProperties");

    if (!vkBackends->destroyInstance ||
        !vkBackends->enumeratePhysicalDevices ||
        !vkBackends->getPhysicalDeviceQueueFamilyProperties)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[vulkan] failed to load vulkan instance level function symbols");

        lvnImplVkTerminate(graphicsctx);
        return Lvn_Result_Failure;
    }

    if (createInfo->presentationModeFlags & Lvn_PresentationModeFlag_Surface)
    {
        vkBackends->getPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
            vkBackends->getInstanceProcAddr(vkBackends->instance, "vkGetPhysicalDeviceSurfaceSupportKHR");

        if (!vkBackends->getPhysicalDeviceSurfaceSupportKHR)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to load vulkan function symbol: vkGetPhysicalDeviceSurfaceSupportKHR");

            lvnImplVkTerminate(graphicsctx);
            return Lvn_Result_Failure;
        }
    }

    // create debug messegenger if debug logging enabled
    if (graphicsctx->enableGraphicsApiDebugLogging)
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

            lvnImplVkTerminate(graphicsctx);
            return Lvn_Result_Failure;
        }

        if (vkBackends->createDebugUtilsMessengerEXT(vkBackends->instance, &debugCreateInfo, NULL, &vkBackends->debugMessenger) != VK_SUCCESS)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[vulkan] failed to create debug message utils");

            lvnImplVkTerminate(graphicsctx);
            return Lvn_Result_Failure;
        }
    }

    return Lvn_Result_Success;
}

void lvnImplVkTerminate(LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    LvnVulkanBackends* vkBackends = (LvnVulkanBackends*) graphicsctx->implData;

    if (vkBackends->debugMessenger)
        vkBackends->destroyDebugUtilsMessengerEXT(vkBackends->instance, vkBackends->debugMessenger, NULL);
    if (vkBackends->instance)
        vkBackends->destroyInstance(vkBackends->instance, NULL);

    if (vkBackends->handle)
        lvn_platformFreeModule(vkBackends->handle);

    lvn_free(vkBackends);
    graphicsctx->implData = NULL;
}
