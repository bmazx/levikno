#include "lvn_impl_egl_loader.h"
#include "lvn_impl_ogl.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifdef LVN_INCLUDE_WAYLAND
    #include <wayland-client.h>
    #include <wayland-egl.h>
#endif

static const char* s_EglLibName = "libEGL.so.1";

static LvnResult lvn_eglCreateSurface(LvnEglLoader* eglLoader, EGLSurface* surface, EGLDisplay eglDisplay, EGLConfig config, void* window, uint32_t widht, uint32_t height);

static LvnResult lvn_eglCreateSurface(
    LvnEglLoader* eglLoader,
    EGLSurface* surface,
    EGLDisplay eglDisplay,
    EGLConfig config,
    void* window,
    uint32_t widht,
    uint32_t height)
{
    LVN_ASSERT(eglLoader && surface, "eglLoader and surface cannot be null");

    LvnWindowPlatformSupport windowSupport = {0};
    lvn_getWindowPlatform(&windowSupport);

#ifdef LVN_INCLUDE_X11
    if (windowSupport.x11Support)
    {
        *surface = eglLoader->eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)window, NULL);
        return Lvn_Result_Success;
    }
#endif

    return Lvn_Result_Failure;
}

LvnResult lvnEglLoaderInit(LvnOpenglBackends* oglBackends, void* display, void* window, uint32_t width, uint32_t height)
{
    LvnEglLoader* eglLoader = (LvnEglLoader*) lvn_calloc(sizeof(LvnEglLoader));

    oglBackends->oglLoader.loaderHandle = eglLoader;

    if (!eglLoader)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to allocate memory for LvnEglLoader");
        goto fail_cleanup;
    }

    // load shared library module
    eglLoader->handle = lvn_platformLoadModule(s_EglLibName);

    if (!eglLoader->handle)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to load vulkan shared library: %s",
                      s_EglLibName);
        goto fail_cleanup;
    }

    // load egl function symbols
    eglLoader->eglGetPlatformDisplay = (PFNEGLGETPLATFORMDISPLAYPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglGetPlatformDisplay");
    eglLoader->eglInitialize = (PFNEGLINITIALIZEPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglInitialize");
    eglLoader->eglChooseConfig = (PFNEGLCHOOSECONFIGPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglChooseConfig");
    eglLoader->eglCreateWindowSurface = (PFNEGLCREATEWINDOWSURFACEPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglCreateWindowSurface");
    eglLoader->eglCreateContext = (PFNEGLCREATECONTEXTPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglCreateContext");
    eglLoader->eglMakeCurrent = (PFNEGLMAKECURRENTPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglMakeCurrent");
    eglLoader->eglSwapBuffers = (PFNEGLSWAPBUFFERSPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglSwapBuffers");
    eglLoader->eglBindAPI = (PFNEGLBINDAPIPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglBindAPI");
    eglLoader->eglGetProcAddress = (PFNEGLGETPROCADDRESSPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglGetProcAddress");

    if (!eglLoader->eglGetPlatformDisplay ||
        !eglLoader->eglInitialize ||
        !eglLoader->eglChooseConfig ||
        !eglLoader->eglCreateWindowSurface ||
        !eglLoader->eglCreateContext ||
        !eglLoader->eglMakeCurrent ||
        !eglLoader->eglSwapBuffers ||
        !eglLoader->eglBindAPI ||
        !eglLoader->eglGetProcAddress)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to load egl function symbols");
        goto fail_cleanup;
    }

    // get platform display
    LvnWindowPlatformSupport windowSupport = {0};
    lvn_getWindowPlatform(&windowSupport);

    EGLenum platformEnum = 0;
    if (windowSupport.waylandSupport)
        platformEnum = EGL_PLATFORM_WAYLAND_KHR;
    else if (windowSupport.x11Support)
        platformEnum = EGL_PLATFORM_X11_KHR;
    else
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to determine window platform enum");
        goto fail_cleanup;
    }

    EGLDisplay eglDisplay = eglLoader->eglGetPlatformDisplay(platformEnum, display, NULL);
    if (eglDisplay == EGL_NO_DISPLAY)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl display");
        goto fail_cleanup;
    }

    // egl initialize
    EGLint major, minor;
    if (!eglLoader->eglInitialize(display, &major, &minor))
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to initialize egl display");
        goto fail_cleanup;
    }
    if (major < 1 || (major == 1 && minor < 5))
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to initialize egl, EGL version 1.5 or higher required, current version: (%d.%d)",
                      major, minor);
        goto fail_cleanup;
    }

    // choose framebuffer config attributes
    EGLint configAttributes[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    eglLoader->eglChooseConfig(eglDisplay, configAttributes, &config, 1, &numConfigs);

    // create surface
    EGLSurface surface;
    if (lvn_eglCreateSurface(eglLoader, &surface, eglDisplay, config, window, width, height) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl surface");
        goto fail_cleanup;
    }

    // bind egl api
    if (!eglLoader->eglBindAPI(EGL_OPENGL_API))
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to bind egl api EGL_OPENGL_API");
        goto fail_cleanup;
    }

    // create context and attributes
    EGLint ctxAttributes[] =
    {
        EGL_CONTEXT_MAJOR_VERSION, 4,
        EGL_CONTEXT_MINOR_VERSION, 5,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#ifdef LVN_CONFIG_DEBUG
        EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
#endif
        EGL_NONE,
    };

    EGLContext eglContext = eglLoader->eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, ctxAttributes);
    if (!eglContext)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl context");
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    lvnEglLoaderTerminate(oglBackends);
    return Lvn_Result_Failure;
}

void lvnEglLoaderTerminate(LvnOpenglBackends* oglBackends)
{
    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->oglLoader.loaderHandle;
    if (!eglLoader)
        return;

    if (eglLoader->handle) lvn_platformFreeModule(eglLoader->handle);

    lvn_free(eglLoader);
    oglBackends->oglLoader.loaderHandle = NULL;
}
