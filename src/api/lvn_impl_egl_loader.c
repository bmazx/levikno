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

    LvnWindowPlatformSupport wps = lvn_getWindowPlatform();

#ifdef LVN_INCLUDE_X11
    if (wps.x11)
    {
        *surface = eglLoader->eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)window, NULL);
        return Lvn_Result_Success;
    }
#endif

    return Lvn_Result_Failure;
}

LvnResult lvnEglLoaderInit(LvnOpenglBackends* oglBackends, void* display, void* window, uint32_t width, uint32_t height)
{
    LVN_ASSERT(oglBackends && display && window, "oglBackends, display, and window cannot be null");

    LvnEglLoader* eglLoader = (LvnEglLoader*) lvn_calloc(sizeof(LvnEglLoader));

    if (!eglLoader)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to allocate memory for LvnEglLoader");
        goto fail_cleanup;
    }

    oglBackends->loaderHandle = eglLoader;

    // load shared library module
    eglLoader->handle = lvn_platformLoadModule(s_EglLibName);

    if (!eglLoader->handle)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to load opengl shared library: %s",
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
    eglLoader->eglDestroySurface = (PFNEGLDESTROYSURFACEPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglDestroySurface");
    eglLoader->eglDestroyContext = (PFNEGLDESTROYCONTEXTPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglDestroyContext");
    eglLoader->eglTerminate = (PFNEGLTERMINATEPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglTerminate");
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
        !eglLoader->eglDestroySurface ||
        !eglLoader->eglDestroyContext ||
        !eglLoader->eglTerminate ||
        !eglLoader->eglGetProcAddress)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to load egl function symbols");
        goto fail_cleanup;
    }

    // get platform display
    LvnWindowPlatformSupport wps = lvn_getWindowPlatform();

    EGLenum platformEnum = 0;
    if (wps.wayland)
        platformEnum = EGL_PLATFORM_WAYLAND_KHR;
    else if (wps.x11)
        platformEnum = EGL_PLATFORM_X11_KHR;
    else
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to determine window platform enum");
        goto fail_cleanup;
    }

    eglLoader->display = eglLoader->eglGetPlatformDisplay(platformEnum, display, NULL);
    if (eglLoader->display == EGL_NO_DISPLAY)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl display");
        goto fail_cleanup;
    }

    // egl initialize
    if (!eglLoader->eglInitialize(eglLoader->display, &eglLoader->versionMajor, &eglLoader->versionMinor))
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to initialize egl display");
        goto fail_cleanup;
    }
    if (eglLoader->versionMajor < 1 || (eglLoader->versionMajor == 1 && eglLoader->versionMinor < 5))
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to initialize egl, EGL version 1.5 or higher required, current version: (%d.%d)",
                      eglLoader->versionMajor, eglLoader->versionMinor);
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

    EGLint numConfigs;
    eglLoader->eglChooseConfig(eglLoader->display, configAttributes, &eglLoader->config, 1, &numConfigs);

    // create surface
    if (lvn_eglCreateSurface(eglLoader, &eglLoader->surface, eglLoader->display, eglLoader->config, window, width, height) != Lvn_Result_Success)
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

    eglLoader->context = eglLoader->eglCreateContext(eglLoader->display, eglLoader->config, EGL_NO_CONTEXT, ctxAttributes);
    if (!eglLoader->context)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl context");
        goto fail_cleanup;
    }

    // get opengl function symbols
    oglBackends->glGetString = (PFNGLGETSTRINGPROC)
        eglLoader->eglGetProcAddress("glGetString");
    oglBackends->glGetError = (PFNGLGETERRORPROC)
        eglLoader->eglGetProcAddress("glGetError");
    oglBackends->glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)
        eglLoader->eglGetProcAddress("glDebugMessageCallback");
    oglBackends->glGetIntegerv = (PFNGLGETINTEGERVPROC)
        eglLoader->eglGetProcAddress("glGetIntegerv");
    oglBackends->glEnable = (PFNGLENABLEPROC)
        eglLoader->eglGetProcAddress("glEnable");
    oglBackends->glEnablei = (PFNGLENABLEIPROC)
        eglLoader->eglGetProcAddress("glEnablei");
    oglBackends->glDisable = (PFNGLDISABLEPROC)
        eglLoader->eglGetProcAddress("glDisable");
    oglBackends->glDisablei = (PFNGLDISABLEIPROC)
        eglLoader->eglGetProcAddress("glDisablei");
    oglBackends->glCreateBuffers = (PFNGLCREATEBUFFERSPROC)
        eglLoader->eglGetProcAddress("glCreateBuffers");
    oglBackends->glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)
        eglLoader->eglGetProcAddress("glDeleteBuffers");
    oglBackends->glCreateSamplers = (PFNGLCREATESAMPLERSPROC)
        eglLoader->eglGetProcAddress("glCreateSamplers");
    oglBackends->glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)
        eglLoader->eglGetProcAddress("glDeleteSamplers");
    oglBackends->glCreateTextures = (PFNGLCREATETEXTURESPROC)
        eglLoader->eglGetProcAddress("glCreateTextures");
    oglBackends->glDeleteTextures = (PFNGLDELETETEXTURESPROC)
        eglLoader->eglGetProcAddress("glDeleteTextures");
    oglBackends->glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)
        eglLoader->eglGetProcAddress("glCreateFramebuffers");
    oglBackends->glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)
        eglLoader->eglGetProcAddress("glDeleteFramebuffers");
    oglBackends->glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)
        eglLoader->eglGetProcAddress("glCreateVertexArrays");
    oglBackends->glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)
        eglLoader->eglGetProcAddress("glDeleteVertexArrays");
    oglBackends->glCreateShader = (PFNGLCREATESHADERPROC)
        eglLoader->eglGetProcAddress("glCreateShader");
    oglBackends->glDeleteShader = (PFNGLDELETESHADERPROC)
        eglLoader->eglGetProcAddress("glDeleteShader");
    oglBackends->glCreateProgram = (PFNGLCREATEPROGRAMPROC)
        eglLoader->eglGetProcAddress("glCreateProgram");
    oglBackends->glDeleteProgram = (PFNGLDELETEPROGRAMPROC)
        eglLoader->eglGetProcAddress("glDeleteProgram");
    oglBackends->glFenceSync = (PFNGLFENCESYNCPROC)
        eglLoader->eglGetProcAddress("glFenceSync");
    oglBackends->glDeleteSync = (PFNGLDELETESYNCPROC)
        eglLoader->eglGetProcAddress("glDeleteSync");
    oglBackends->glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)
        eglLoader->eglGetProcAddress("glClientWaitSync");
    oglBackends->glWaitSync = (PFNGLWAITSYNCPROC)
        eglLoader->eglGetProcAddress("glWaitSync");
    oglBackends->glFlush = (PFNGLFLUSHPROC)
        eglLoader->eglGetProcAddress("glFlush");
    oglBackends->glCheckNamedFramebufferStatus = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)
        eglLoader->eglGetProcAddress("glCheckNamedFramebufferStatus");
    oglBackends->glNamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)
        eglLoader->eglGetProcAddress("glNamedFramebufferTexture");
    oglBackends->glNamedFramebufferDrawBuffer = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC)
        eglLoader->eglGetProcAddress("glNamedFramebufferDrawBuffer");
    oglBackends->glNamedFramebufferDrawBuffers = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)
        eglLoader->eglGetProcAddress("glNamedFramebufferDrawBuffers");
    oglBackends->glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)
        eglLoader->eglGetProcAddress("glSamplerParameteri");
    oglBackends->glTextureParameteri = (PFNGLTEXTUREPARAMETERIPROC)
        eglLoader->eglGetProcAddress("glTextureParameteri");
    oglBackends->glTextureStorage2D = (PFNGLTEXTURESTORAGE2DPROC)
        eglLoader->eglGetProcAddress("glTextureStorage2D");
    oglBackends->glTextureStorage2DMultisample = (PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)
        eglLoader->eglGetProcAddress("glTextureStorage2DMultisample");
    oglBackends->glTextureSubImage2D = (PFNGLTEXTURESUBIMAGE2DPROC)
        eglLoader->eglGetProcAddress("glTextureSubImage2D");
    oglBackends->glShaderSource = (PFNGLSHADERSOURCEPROC)
        eglLoader->eglGetProcAddress("glShaderSource");
    oglBackends->glCompileShader = (PFNGLCOMPILESHADERPROC)
        eglLoader->eglGetProcAddress("glCompileShader");
    oglBackends->glGetShaderiv = (PFNGLGETSHADERIVPROC)
        eglLoader->eglGetProcAddress("glGetShaderiv");
    oglBackends->glAttachShader = (PFNGLATTACHSHADERPROC)
        eglLoader->eglGetProcAddress("glAttachShader");
    oglBackends->glLinkProgram = (PFNGLLINKPROGRAMPROC)
        eglLoader->eglGetProcAddress("glLinkProgram");
    oglBackends->glGetProgramiv = (PFNGLGETPROGRAMIVPROC)
        eglLoader->eglGetProcAddress("glGetProgramiv");
    oglBackends->glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)
        eglLoader->eglGetProcAddress("glGetProgramInfoLog");
    oglBackends->glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)
        eglLoader->eglGetProcAddress("glGetShaderInfoLog");
    oglBackends->glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)
        eglLoader->eglGetProcAddress("glBlendFuncSeparatei");
    oglBackends->glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)
        eglLoader->eglGetProcAddress("glBlendEquationSeparatei");
    oglBackends->glColorMaski = (PFNGLCOLORMASKIPROC)
        eglLoader->eglGetProcAddress("glColorMaski");
    oglBackends->glPolygonMode = (PFNGLPOLYGONMODEPROC)
        eglLoader->eglGetProcAddress("glPolygonMode");
    oglBackends->glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)
        eglLoader->eglGetProcAddress("glNamedBufferStorage");
    oglBackends->glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)
        eglLoader->eglGetProcAddress("glNamedBufferData");
    oglBackends->glMapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)
        eglLoader->eglGetProcAddress("glMapNamedBufferRange");
    oglBackends->glUnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC)
        eglLoader->eglGetProcAddress("glUnmapNamedBuffer");
    oglBackends->glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC)
        eglLoader->eglGetProcAddress("glEnableVertexArrayAttrib");
    oglBackends->glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)
        eglLoader->eglGetProcAddress("glVertexArrayAttribBinding");
    oglBackends->glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC)
        eglLoader->eglGetProcAddress("glVertexArrayAttribFormat");
    oglBackends->glVertexArrayAttribIFormat = (PFNGLVERTEXARRAYATTRIBIFORMATPROC)
        eglLoader->eglGetProcAddress("glVertexArrayAttribIFormat");
    oglBackends->glVertexArrayAttribLFormat = (PFNGLVERTEXARRAYATTRIBLFORMATPROC)
        eglLoader->eglGetProcAddress("glVertexArrayAttribLFormat");
    oglBackends->glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)
        eglLoader->eglGetProcAddress("glVertexArrayVertexBuffer");
    oglBackends->glVertexArrayVertexBuffers = (PFNGLVERTEXARRAYVERTEXBUFFERSPROC)
        eglLoader->eglGetProcAddress("glVertexArrayVertexBuffers");
    oglBackends->glUseProgram = (PFNGLUSEPROGRAMPROC)
        eglLoader->eglGetProcAddress("glUseProgram");
    oglBackends->glBindBuffer = (PFNGLBINDBUFFERPROC)
        eglLoader->eglGetProcAddress("glBindBuffer");
    oglBackends->glBindVertexBuffers = (PFNGLBINDVERTEXBUFFERSPROC)
        eglLoader->eglGetProcAddress("glBindVertexBuffers");
    oglBackends->glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)
        eglLoader->eglGetProcAddress("glBindVertexArray");
    oglBackends->glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)
        eglLoader->eglGetProcAddress("glBindFramebuffer");
    oglBackends->glClear = (PFNGLCLEARPROC)
        eglLoader->eglGetProcAddress("glClear");
    oglBackends->glClearColor = (PFNGLCLEARCOLORPROC)
        eglLoader->eglGetProcAddress("glClearColor");
    oglBackends->glClearNamedFramebufferiv = (PFNGLCLEARNAMEDFRAMEBUFFERIVPROC)
        eglLoader->eglGetProcAddress("glClearNamedFramebufferiv");
    oglBackends->glClearNamedFramebufferuiv = (PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC)
        eglLoader->eglGetProcAddress("glClearNamedFramebufferuiv");
    oglBackends->glClearNamedFramebufferfv = (PFNGLCLEARNAMEDFRAMEBUFFERFVPROC)
        eglLoader->eglGetProcAddress("glClearNamedFramebufferfv");
    oglBackends->glClearNamedFramebufferfi = (PFNGLCLEARNAMEDFRAMEBUFFERFIPROC)
        eglLoader->eglGetProcAddress("glClearNamedFramebufferfi");
    oglBackends->glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)
        eglLoader->eglGetProcAddress("glDrawArraysInstancedBaseInstance");
    oglBackends->glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)
        eglLoader->eglGetProcAddress("glDrawElementsInstancedBaseVertexBaseInstance");
    oglBackends->glDepthRange = (PFNGLDEPTHRANGEPROC)
        eglLoader->eglGetProcAddress("glDepthRange");
    oglBackends->glViewport = (PFNGLVIEWPORTPROC)
        eglLoader->eglGetProcAddress("glViewport");
    oglBackends->glScissor = (PFNGLSCISSORPROC)
        eglLoader->eglGetProcAddress("glScissor");

    // bind function pointers
    oglBackends->ogllCreateSurface = lvnEglCreateSurface;
    oglBackends->ogllDestroySurface = lvnEglDestroySurface;
    oglBackends->ogllMakeCurrent = lvnEglMakeCurrent;
    oglBackends->ogllSwapBuffers = lvnEglSwapBuffers;

    eglLoader->eglMakeCurrent(eglLoader->display, eglLoader->surface, eglLoader->surface, eglLoader->context);

    return Lvn_Result_Success;

fail_cleanup:
    lvnEglLoaderTerminate(oglBackends);
    return Lvn_Result_Failure;
}

void lvnEglLoaderTerminate(LvnOpenglBackends* oglBackends)
{
    LVN_ASSERT(oglBackends, "oglBackends cannot be null");

    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;
    if (!eglLoader)
        return;

    if (eglLoader->context)
    {
        eglLoader->eglDestroyContext(eglLoader->display, eglLoader->context);
        eglLoader->context = NULL;
    }
    if (eglLoader->surface)
    {
        eglLoader->eglDestroySurface(eglLoader->display, eglLoader->surface);
        eglLoader->surface = NULL;
    }

    eglLoader->eglTerminate(eglLoader->display);

    if (eglLoader->handle) lvn_platformFreeModule(eglLoader->handle);

    lvn_free(eglLoader);
    oglBackends->loaderHandle = NULL;
}

LvnResult lvnEglCreateSurface(const LvnOpenglBackends* oglBackends, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo)
{
    LVN_ASSERT(oglBackends && surface && createInfo, "oglBackends, surface, and createInfo cannot be null");

    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;

    EGLSurface eglSurface;
    if (lvn_eglCreateSurface(eglLoader, &eglSurface, eglLoader->display, eglLoader->config, createInfo->nativeWindowHandle, 1, 1) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl surface for surface object at %p",
                      surface);
        return Lvn_Result_Failure;
    }

    surface->surfaceData = eglSurface;

    return Lvn_Result_Success;
}

void lvnEglDestroySurface(const LvnOpenglBackends* oglBackends, LvnSurface* surface)
{
    LVN_ASSERT(oglBackends && surface, "oglBackends and surface cannot be null");

    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;
    EGLSurface eglSurface = (EGLSurface) surface->surfaceData;

    eglLoader->eglDestroySurface(eglLoader->display, eglSurface);
    surface->surfaceData = NULL;
}

void lvnEglMakeCurrent(const LvnOpenglBackends* oglBackends, LvnSurface* surface)
{
    LVN_ASSERT(oglBackends, "oglBackends cannot be null");
    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;
    EGLSurface eglSurface = (EGLSurface) surface->surfaceData;

    eglLoader->eglMakeCurrent(eglLoader->display, eglSurface, eglSurface, eglLoader->context);
}

void lvnEglSwapBuffers(const LvnOpenglBackends* oglBackends, LvnSurface* surface)
{
    LVN_ASSERT(oglBackends, "oglBackends cannot be null");
    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;
    EGLSurface eglSurface = (EGLSurface) surface->surfaceData;

    eglLoader->eglSwapBuffers(eglLoader->display, eglSurface);
}
