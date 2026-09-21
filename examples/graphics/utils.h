#ifndef HG_LVN_GRAPHICS_UTIL_H_
#define HG_LVN_GRAPHICS_UTIL_H_

#include <levikno/levikno.h>

#include <GLFW/glfw3.h>

#if defined(LVN_PLATFORM_LINUX)
    #define GLFW_EXPOSE_NATIVE_WAYLAND
    #define GLFW_EXPOSE_NATIVE_X11
#elif defined(LVN_PLATFORM_WINDOWS)
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

#include <stdio.h>

typedef struct NativeWindowData
{
    void* display;
    void* window;
} NativeWindowData;

typedef struct WindowData
{
    bool framebufferResized;
    int width, height;
} WindowData;
static WindowData s_WindowData = {0};

static size_t s_MemAllocCount = 0;

static void* customMalloc(size_t size, void* userData)
{
    if (!size) { return NULL; }
    void* ptr = malloc(size);
    if (!ptr) { printf("alloc fail\n"); exit(-1); }
    s_MemAllocCount++;
    return ptr;
}

static void customFree(void* ptr, void* userData)
{
    if (!ptr) { return; }
    s_MemAllocCount--;
    free(ptr);
}

static void* customRealloc(void* ptr, size_t size, void* userData)
{
    if (!ptr) { return customMalloc(size, userData); }
    void* newptr = realloc(ptr, size);
    if (!newptr) { printf("realloc fail\n"); exit(-1); }
    return newptr;
}

static inline void GLFWerrorCallback(int error, const char* descripion)
{
    printf("[glfw]: (%d): %s\n", error, descripion);
}

static inline void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    WindowData* winData = (WindowData*) glfwGetWindowUserPointer(window);
    winData->framebufferResized = true;
    winData->width = width;
    winData->height = height;
}

static inline GLFWwindow* initWindow(int width, int height, const char* title)
{
    if (!glfwInit())
        return NULL;

    GLFWwindow* window;
    glfwSetErrorCallback(GLFWerrorCallback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return NULL;
    }

    glfwSetWindowSizeLimits(window, 400, 300, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetWindowUserPointer(window, &s_WindowData);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    return window;
}

static inline void terminateWindow(GLFWwindow* window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

static inline NativeWindowData getNativeWindowData(GLFWwindow* window)
{
    NativeWindowData nativeData = {0};

#if defined(LVN_PLATFORM_LINUX)
    LvnWindowPlatformSupport wps = lvnGetWindowPlatformSupport();

    if (wps.wayland)
    {
        struct wl_display* nativeDisplay = glfwGetWaylandDisplay();
        struct wl_surface* nativeWindow = glfwGetWaylandWindow(window);
        nativeData.display = (void*)nativeDisplay;
        nativeData.window = (void*)nativeWindow;
    }
    else if (wps.x11)
    {
        Display* nativeDisplay = glfwGetX11Display();
        Window nativeWindow = glfwGetX11Window(window);
        nativeData.display = (void*)nativeDisplay;
        nativeData.window = (void*)nativeWindow;
    }
#elif defined(LVN_PLATFORM_WINDOWS)
    HWND nativeWindow = glfwGetWin32Window(window);
    HINSTANCE nativeDisplay = (HINSTANCE)GetWindowLongPtr(nativeWindow, GWLP_HINSTANCE);
    nativeData.display = (void*)nativeDisplay;
    nativeData.window = (void*)nativeWindow;
#endif

    return nativeData;
}

#endif // !HG_LVN_GRAPHICS_UTIL_H_
